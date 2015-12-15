#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <mxml.h>
#include <wiringPi.h>
#include "manchester.h"


#define           poly                  0x1021          /* crc-ccitt mask */
#define           CRC_initial_xmodem    0x0000          /* crc-ccitt initial value for xmodem */
#define           CRC_initial_CCITT     0xFFFF          /* crc-ccitt initial value for standard */
//#define           DEBUG
#define           NO DEBUG

struct s_ParametersOfMCZ {
   int Id;
   int Modes;
   int User;
   int Puissance;
   int Ventilateur1;
   int Ventilateur2;
};

struct s_Octet {
   short int O_MSB_Id;
   short int O_ISB_Id;
   short int O_LSB_Id;
   short int O_Parameter1;
   short int O_Parameter2;
   short int O_MSB_CRC;
   short int O_LSB_CRC;
};

struct s_Packet {
   short int *num_packet;
   int tabPacket[7];
};

/* global variables */ 
unsigned short good_crc; 
unsigned short bad_crc; 
unsigned short text_length;
unsigned short emit = 0;

struct s_ParametersOfMCZ Util_config = {0}; /* declaration d'une structure vide */
struct s_ParametersOfMCZ Old_config = {0}; /* declaration d'une structure vide */
struct s_Octet octet = {0};                /* declaration d'une structure vide */
struct s_Packet packet = {0};              /* declaration d'une structure vide */
/*
 * Utilise mini-xml pour charger dans un arbre en memoire le contenu du fichier FileName
 * FileName : chemin du fichier à ouvrir
 * return : Pointeur sur le noeud racine de l'arbre
 */
mxml_node_t *loadXmlTree(const char *FileName)
{
   FILE *fp;
   mxml_node_t *tree;

   fp = fopen(FileName, "r");
   if(fp == NULL) {
      printf("Erreur ouverture en lecture %s :\n",FileName);
      perror("fopen");
      return NULL;
   }
   tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
   if(tree == NULL)
      printf("Erreur lecture xml %s\n",FileName);
   fclose(fp);
   return tree;
}

void CompartParameters(struct s_ParametersOfMCZ *oldParam, struct s_ParametersOfMCZ *newParam)
{
      if (oldParam->Modes == newParam->Modes & oldParam->Puissance == newParam->Puissance &
          oldParam->Ventilateur1 == newParam->Ventilateur1 & oldParam->Ventilateur2 == newParam->Ventilateur2 )
         emit = 0;
      else 
         emit = 1;
           
      if (emit == 1){

         printf("changement \n");
         oldParam->Modes = newParam->Modes;
         oldParam->User = newParam->User;
         oldParam->Puissance = newParam->Puissance;
         oldParam->Ventilateur1 = newParam->Ventilateur1;
         oldParam->Ventilateur2 = newParam->Ventilateur2;
      }

}

void parseParameters(mxml_node_t *tree, struct s_ParametersOfMCZ *pParam)
{
   char *SubValue, *SubTag = NULL;
   mxml_node_t *node = mxmlFindElement(tree, tree, "ParametersOfMCZ", NULL, NULL, MXML_DESCEND);
   mxml_node_t *fils=node->child;
   int name;

   while(fils != NULL) {
      if ((fils->type != MXML_ELEMENT)
       ||(fils->child == NULL)) {
         fils = fils->next;
         continue;
      }
      SubValue = fils->child->value.opaque;
      SubTag=fils->value.element.name;

      if (strcmp (SubTag, "Id") == 0)
         pParam->Id = strtol(SubValue, NULL, 10);
      else if (strcmp (SubTag, "Modes") == 0)
         pParam->Modes = strtol(SubValue, NULL, 10);
      else if (strcmp (SubTag, "User") == 0)
         pParam->User = strtol(SubValue, NULL, 10);
      else if (strcmp (SubTag, "Puissance") == 0)
         pParam->Puissance = strtol(SubValue, NULL, 10);
      else if (strcmp (SubTag, "Ventilateur1") == 0)
         pParam->Ventilateur1 = strtol(SubValue, NULL, 10);
      else if (strcmp (SubTag, "Ventilateur2") == 0)
         pParam->Ventilateur2 = strtol(SubValue, NULL, 10);
      fils=fils->next;	 
      }
}
/***************************************************************************
              Converti les donnees XML en 5 octets.   
    Entree : Structure de ParametersOfMCZ
    sortie : 5 premiers octets de la trame
/***************************************************************************/
void Convert2Databytes(struct s_ParametersOfMCZ *pParam, struct s_Octet *oct)
{
    
   oct->O_MSB_Id = ((pParam->Id & 0xFF0000)>>16) & 0xFF;
   oct->O_ISB_Id = ((pParam->Id & 0xFF00)>>8) & 0xFF;
   oct->O_LSB_Id = (pParam->Id & 0xFF);
   oct->O_Parameter1 = (pParam->Ventilateur1 <<5 | 
					   pParam->Puissance <<2 | 
					   pParam->Modes) & 0xFF;
   oct->O_Parameter2 = (pParam->User <<6| pParam->Ventilateur2 << 3) & 0xFF;
}
/***************************************************************************
              Calcul du CRC-CCITT16.   
    Entree : Structure de s_Octet
    sortie : 2 octets de fin de trame contenant le CRC
/***************************************************************************/
void Calcul_Crc(struct s_Octet *oct) 
{ 
    void update_good_crc(unsigned short); 
    void augment_message_for_good_crc(); 
    void update_bad_crc(unsigned short);

    unsigned short i;
    unsigned short data[6];
	
	data[0] = oct->O_MSB_Id;
	data[1] = oct->O_ISB_Id;
	data[2] = oct->O_LSB_Id;
	data[3] = oct->O_Parameter1;
	data[4] = oct->O_Parameter2;
    good_crc = CRC_initial_xmodem; 
    bad_crc = 0xffff; 
    i = 0; 
    text_length= 0; 
	for (i=0; i<5; i++)
    {
        //printf("%02X\n",data[i]); 
        update_good_crc(data[i]); 
        update_bad_crc(data[i]); 
        text_length++;
    } 
    augment_message_for_good_crc();
#ifdef DEBUG
     printf("good_CRC = %04X  Bad_CRC = %04X  Length = %u\n",
            good_crc,bad_crc,text_length,data); 
#endif /* DEBUG */
    oct->O_MSB_CRC = (good_crc & 0xFF00)>>8;
    oct->O_LSB_CRC = good_crc & 0xFF;
}

void update_good_crc(unsigned short ch) 
{ 
    unsigned short i, v, xor_flag;

    /* 
    Align test bit with leftmost bit of the message byte. 
    */ 
    v = 0x80;

    for (i=0; i<8; i++) 
    { 
        if (good_crc & 0x8000) 
        { 
            xor_flag= 1; 
        } 
        else 
        { 
            xor_flag= 0; 
        } 
        good_crc = good_crc << 1;

        if (ch & v) 
        { 
            /* 
            Append next bit of message to end of CRC if it is not zero. 
            The zero bit placed there by the shift above need not be 
            changed if the next bit of the message is zero. 
            */ 
            good_crc= good_crc + 1; 
        }

        if (xor_flag) 
        { 
            good_crc = good_crc ^ poly; 
        }

        /* 
        Align test bit with next bit of the message byte. 
        */ 
        v = v >> 1; 
    } 
}

void augment_message_for_good_crc() 
{ 
    unsigned short i, xor_flag;

    for (i=0; i<16; i++) 
    { 
        if (good_crc & 0x8000) 
        { 
            xor_flag= 1; 
        } 
        else 
        { 
            xor_flag= 0; 
        } 
        good_crc = good_crc << 1;

        if (xor_flag) 
        { 
            good_crc = good_crc ^ poly; 
        } 
    } 
}

void update_bad_crc(unsigned short ch) 
{ 
    /* based on code found at 
    http://www.programmingparadise.com/utility/crc.html 
    */

    unsigned short i, xor_flag;

    /* 
    Why are they shifting this byte left by 8 bits?? 
    How do the low bits of the poly ever see it? 
    */ 
    ch<<=8;

    for(i=0; i<8; i++) 
    { 
        if ((bad_crc ^ ch) & 0x8000) 
        { 
            xor_flag = 1; 
        } 
        else 
        { 
            xor_flag = 0; 
        } 
        bad_crc = bad_crc << 1; 
        if (xor_flag) 
        { 
            bad_crc = bad_crc ^ poly; 
        } 
        ch = ch << 1; 
    } 
}

/***************************************************************************
              Calcul de la parite d'un octet.
    Entree : octet b
    sortie : 1 impaire 0 paire
/***************************************************************************/
int parite_paire(unsigned short nombre) { //retourne 0 si paire, 1 si impaire
   unsigned int ret=0;
 
   while(nombre) {
      ret^=nombre & 1;
      nombre>>=1;
   }
   return ret;
}
/***************************************************************************
              Encapsulation des octets en paquets de 12bits.
    Entree : octet "oct"
    sortie : packet 12bits
/***************************************************************************/
void encapsule(struct s_Octet *oct, struct s_Packet *Pack)
{
	unsigned short i, par;
	unsigned short data[7];
    unsigned short first=1;
    int inter;
	
	data[0] = oct->O_MSB_Id;
	data[1] = oct->O_ISB_Id;
	data[2] = oct->O_LSB_Id;
	data[3] = oct->O_Parameter1;
	data[4] = oct->O_Parameter2;
	data[5] = oct->O_MSB_CRC;
	data[6] = oct->O_LSB_CRC;
	for (i=0; i<7; i++)
    {
        if (first == 1){
          inter = ((data[i] <<1) | 0x1) & 0x1FF;
          par = parite_paire(inter);
          Pack->tabPacket[i] = ((0x1 <<11) | (inter<<2) | (par<<1) | 0x1) & 0xFFF;
          first = 0;
        }
        else {
          inter = ((data[i] <<1) | 0x0) & 0x1FF;
          par = parite_paire(inter);
          Pack->tabPacket[i] = ((0x1 <<11) | (inter<<2) | (par<<1) | 0x1) & 0xFFF;

        }
    }
}
/***************************************************************************
              Envoie des trames de données codées en "manchester".
    Entree : packet 12bits et n nombre de repetition
    
/***************************************************************************/
void manchester_send_trame(struct s_Packet *Pack,int n) {

  int i, lenght;
  unsigned char txPin = 0;

  lenght = n;
  digitalWrite(txPin, HIGH);
  delayMicroseconds(DEBUT_COMMANDE);
  while(n>0) {
    //digitalWrite(txPin, LOW);
#ifdef DEBUG  
    printf(" trame %d: \n",(lenght+1)-n);
#endif /* DEBUG */	
    for (i=0; i<7; i++){
        //digitalWrite(txPin, HIGH);
        manchester_send(Pack->tabPacket[i]);
        //delayMicroseconds(INTER_DONNEE);
#ifdef DEBUG	
	printf(" %x \n",Pack->tabPacket[i]);
#endif /* DEBUG */	
	}
#ifdef DEBUG	
   printf("Delai intertrame \n");
#endif /* DEBUG */	
   //digitalWrite(txPin, LOW);
   delayMicroseconds(INTER_TRAME);
    n--;
  }
#ifdef DEBUG
 printf(" Commande envoyee \n"); 
#endif /* DEBUG */
}


/**************************************************************************
                Fonction de passage du programme en temps .
**************************************************************************/
void scheduler_realtime() {
	struct sched_param p;
	p.__sched_priority = sched_get_priority_max(SCHED_RR);
	if( sched_setscheduler( 0, SCHED_RR, &p ) == -1 ) {
		perror("Failed to switch to realtime scheduler.");
	}
}


/*************************************************************************
              Fonction de remise du programme en temps standard
*************************************************************************/
void scheduler_standard() {
	struct sched_param p;
	p.__sched_priority = 0;
	if( sched_setscheduler( 0, SCHED_OTHER, &p ) == -1 ) {
		perror("Failed to switch to normal scheduler.");
	}
}


/*************************************************************************
                                  MAIN
*************************************************************************/
int main(void){


   while (1) {
   /* Pointe vers l'emplacement du fichier xml */
   mxml_node_t *tree = loadXmlTree("/usr/share/nginx/www/MCZ/cgi-bin/parameters.xml");

 
   if(tree == NULL)
      return -1;
   parseParameters(tree, &Util_config);
   mxmlDelete(tree);

#ifdef DEBUG 
   /* Display in-application configuration */
   printf("Parametres : \n");
   printf("id = 0x%x\n", Util_config.Id);
   printf("Mode = 0x%x\n", Util_config.Modes);
   printf("User = 0x%x\n", Util_config.User);
   printf("Puissance de chauffe = 0x%x\n", Util_config.Puissance);
   printf("vitesse du Ventilateur1 = 0x%x\n", Util_config.Ventilateur1);
   printf("vitesse du Ventilateur2 = 0x%x\n", Util_config.Ventilateur2);
#endif /* DEBUG */
   Convert2Databytes(&Util_config,&octet);
   Calcul_Crc(&octet);
#ifdef DEBUG
   printf("octet 1: 0x%x\n",octet.O_MSB_Id);
   printf("octet 2: 0x%x\n",octet.O_ISB_Id);
   printf("octet 3: 0x%x\n",octet.O_LSB_Id);
   printf("octet 4: 0x%x\n",octet.O_Parameter1);
   printf("octet 5: 0x%x\n",octet.O_Parameter2);
   printf("octet 6: 0x%x\n",octet.O_MSB_CRC);
   printf("octet 7: 0x%x\n",octet.O_LSB_CRC);
#endif /* DEBUG */
   encapsule(&octet,&packet);
#ifdef DEBUG
   printf("octet 1 encapsule: 0x%x\n",packet.tabPacket[0]);
   printf("octet 2 encapsule: 0x%x\n",packet.tabPacket[1]);
   printf("octet 3 encapsule: 0x%x\n",packet.tabPacket[2]);
   printf("octet 4 encapsule: 0x%x\n",packet.tabPacket[3]);
   printf("octet 5 encapsule: 0x%x\n",packet.tabPacket[4]);
   printf("octet 6 encapsule: 0x%x\n",packet.tabPacket[5]);
   printf("octet 7 encapsule: 0x%x\n",packet.tabPacket[6]);
#endif /* DEBUG */
#ifdef DEBUG
   printf("Parametres : \n");
   printf("id = 0x%x\n", Old_config.Id);
   printf("Mode = 0x%x\n", Old_config.Modes);
   printf("User = 0x%x\n", Old_config.User);
   printf("Puissance de chauffe = 0x%x\n", Old_config.Puissance);
   printf("vitesse du Ventilateur1 = 0x%x\n", Old_config.Ventilateur1);
   printf("vitesse du Ventilateur2 = 0x%x\n", Old_config.Ventilateur2);
#endif /* DEBUG */

   // Comparaison des parametres
   CompartParameters(&Old_config, &Util_config);
   // printf("%d",emit);



        if (emit){

        // Send the data
        manchester_init();
        printf("envoie commande \n");
        //On passe en temps reel
        scheduler_realtime();
        
        // envoi de la commande RF
        manchester_send_trame(&packet,5);
   
        //On revient en mode normal
        scheduler_standard();
     
       }
   sleep(1.5);
   }
   printf("sortie d'application MCZ");
   return 0;
}

#!/usr/bin/perl -w
     # Tell perl to send a html header.
     # So your browser gets the output
     # rather then <stdout>(command line
     # on the server.)
use CGI qw(:standard);
use strict;
use XML::LibXML;

 my $query = new CGI;
 my $valeur_puis = $query->param('Puis'); #donnee d'entree "Puis"
 my $filename = "parameters.xml";
 my $p = XML::LibXML->new();
 my $d = $p->parse_file($filename);

for my $node ($d->findnodes('//ParametersOfMCZ'))
{
   my ($puissance_node) = $node->findnodes('Puissance/text()')
      or next;
   $puissance_node->setData($valeur_puis);
}
print $d->toString; #Affiche
print $d->toFile($filename,0);

print "Content-type: text/html\n\n";
     # print your basic html tags.
     # and the content of them.
# Lancement de la commande RF vers le MCZ en "one shoot"     
system "sudo /home/pi/Dev/APPLI_MCZ/src/Appli_Cmd_Mcz"

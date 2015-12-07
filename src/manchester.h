#ifndef MANCHESTER_H
#define MANCHESTER_H

#include <inttypes.h>


// in microseconds
//#define INTER_DONNEE    833  /* 833 us */
//#define HALF_BIT        416  /* 416 us /2 */
//#define INTER_TRAME     20000 /* 20 ms */
//#define DEBUT_COMMANDE  100000 /* 100ms */
#define INTER_DONNEE    860  /* 860 us */
#define HALF_BIT        390  /* 390 us /2 */
#define INTER_TRAME     5080 /* 5,08 ms */
#define DEBUT_COMMANDE  100000 /* 100ms */

int manchester_init();
void manchester_send(uint16_t t);
//uint16_t manchester_read();

// low level, for debug
//int8_t manchester_wait_bit();
void manchester_send_bit(int8_t b);
//void manchester_send_trame(int *tab,int n);
#endif /* MANCHESTER_H */

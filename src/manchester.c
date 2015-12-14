#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include "manchester.h"

unsigned char manchesterPinOut = 0;
unsigned char manchesterPinIn = 2;


int manchester_init() {

    int status;
    status = wiringPiSetup();
    if (status != -1) {
        pinMode(manchesterPinIn, INPUT);
        pinMode(manchesterPinOut, OUTPUT);
    } else {
        printf("GPIO setup failed %d\n", status);
    }
    return status;
}

void manchester_send_bit(int8_t b) {
    if (b) {
	digitalWrite(manchesterPinOut, HIGH);
	delayMicroseconds(HALF_BIT);
	digitalWrite(manchesterPinOut, LOW);
	delayMicroseconds(HALF_BIT);
    } else {
	digitalWrite(manchesterPinOut, LOW);
	delayMicroseconds(HALF_BIT);
	digitalWrite(manchesterPinOut, HIGH);
	delayMicroseconds(HALF_BIT);
    }
#ifdef DEBUG
   printf("%d",b);
#endif /* DEBUG */
}

// send 12-bit number in Manchester code.
void manchester_send(uint16_t t) {
    int mask = 0x800;
//    int ones = 0;
    manchester_send_bit(0); /* bit de preambule */
    delayMicroseconds(HALF_BIT);
    for (int i = 0; i < 12; ++i) {
	int bit =  !! (t & mask);
	manchester_send_bit(bit);
	mask >>= 1;
//	if (bit == 1) {
//	    ones ++;
//	} else {
//	    ones = 0;
//	}
//	if (ones == 2) {
//	    manchester_send_bit(0);
//	    ones = 0;
//	}
    }
//    if (ones > 0) {
//	manchester_send_bit(0);
//    }
}

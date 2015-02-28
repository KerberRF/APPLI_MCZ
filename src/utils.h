#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#define REAL_TIME 0
#define STANDARD  1

/**
 * Calculate a duration
 *
 * @param start start of the computing or null if
 * this is the first time this function is inoked
 *
 * @return current time
 */
struct timeval* showTime(struct timeval* start);

/**
 * Switch to real-time mode
 *
 * @param mode (REAL_TIME | STANDARD)
 */
void cpuMode(int mode);

/**
 * Exit from real-time mode
 */
void scheduler_standard();


#endif

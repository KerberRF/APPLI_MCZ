#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <malloc.h>
#include <sched.h>
#include "utils.h"



/**
 * Calculate a duration
 *
 * @param start start of the computing or null if
 * this is the first time this function is inoked
 *
 * @return current time
 */
struct timeval* showTime(struct timeval* start)
{
    struct timeval* stop;
    long tv_sec;
    long tv_usec;
    stop = (struct timeval*)malloc(sizeof(struct timeval));
    gettimeofday(stop, 0);
    if (!start) return stop;
    tv_sec = (start->tv_usec > stop->tv_usec) ? stop->tv_sec - start->tv_sec - 1 : stop->tv_sec - start->tv_sec;
    tv_usec = (start->tv_usec > stop->tv_usec) ? 1000000 + stop->tv_usec - start->tv_usec : stop->tv_usec - start->tv_usec;
    fprintf(stderr, "%lus %lums %luµs\n", tv_sec, tv_usec/1000, tv_sec % 1000);
    return stop;
}

/**
 * Switch between standard and  real-time mode
 *
 * @param mode (REAL_TIME | STANDARD)
 */
void cpuMode(int mode)
{
    struct sched_param p;
    switch (mode) {
        case REAL_TIME:
            p.__sched_priority = sched_get_priority_max(SCHED_RR);
            if( sched_setscheduler( 0, SCHED_RR, &p ) == -1 ) {
                perror("Failed to switch to realtime scheduler.");
            }
            break;
        case STANDARD:
        default:
            p.__sched_priority = 0;
            if( sched_setscheduler( 0, SCHED_OTHER, &p ) == -1 ) {
                perror("Failed to switch to normal scheduler.");
            }
            break;
    }
}

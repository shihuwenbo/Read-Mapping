#include "misc.h"
#include <stdlib.h>
#include <sys/time.h>

void start_timer(double* time)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int t = 1000000 * tv.tv_sec + tv.tv_usec;
    *time = t / 1000000.0;
}

void stop_timer(double* time)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int t = 1000000 * tv.tv_sec + tv.tv_usec;
    double stop_time = (double) t;
    *time = (stop_time / 1000000.0) - *time;
}

int randnum(int low, int high)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec);
    return rand() % (high - low + 1) + low;
}

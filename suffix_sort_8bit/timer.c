#include "timer.h"
#include <stdlib.h>

void start_timer(int* time)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    *time = tv.tv_usec;
}

void stop_timer(int* time)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int stop_time = tv.tv_usec;
    *time = stop_time - *time;
}

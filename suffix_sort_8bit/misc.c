#include "misc.h"
#include <stdlib.h>
#include <sys/time.h>

// start timer
void start_timer(double* time)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int t = 1000000 * tv.tv_sec + tv.tv_usec;
    *time = t / 1000000.0;
}

// stop timer
void stop_timer(double* time)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned int t = 1000000 * tv.tv_sec + tv.tv_usec;
    double stop_time = (double) t;
    *time = (stop_time / 1000000.0) - *time;
}

// generate random number between low and high
int randnum(int low, int high)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec);
    return rand() % (high - low + 1) + low;
}

// get the pos-th bit in the array bit_vector
inline int get_bit(char* bit_vector, int pos)
{
    int idx = pos / 8;
    int boff = pos % 8;
    unsigned char byte = bit_vector[idx];
    byte = byte << boff;
    byte = byte >> 7;
    return byte;
}

// set the pos-th bit in the bit_vector to 1
inline void set_bit(char* bit_vector, int pos)
{
    int idx = pos / 8;
    int boff = pos % 8;
    unsigned char mask = 1 << (8 - boff -1);
    bit_vector[idx] = bit_vector[idx] | mask;
}

// clear the pos-th bit in the bit_vector to 0
inline void clear_bit(char* bit_vector, int pos)
{
    int idx = pos / 8;
    int boff = pos % 8;
    unsigned char mask = ~ (1 << (8 - boff -1));
    bit_vector[idx] = bit_vector[idx] & mask;
}

// given a letter, return its rank in the alphabet
inline int alpha_rank(char l)
{
    // rank alphabetically $<a<c<g<t
    switch(l)
    {
        case '$': return 0;
        case 'a': return 1;
        case 'c': return 2;
        case 'g': return 3;
        case 't': return 4;
    }
    return -1;
}


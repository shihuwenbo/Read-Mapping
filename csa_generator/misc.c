#include "misc.h"
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>

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

// get the bp at pos
char get_bp_2bit(char* genome, unsigned int pos)
{
    unsigned int byte_pos = pos / (BYTE_SIZE / ENCODE_SIZE_2BIT);
    unsigned int byte_off = pos % (BYTE_SIZE / ENCODE_SIZE_2BIT);
    unsigned char dna = genome[byte_pos];
    dna = dna << (byte_off * ENCODE_SIZE_2BIT);
    dna = dna >> (BYTE_SIZE - ENCODE_SIZE_2BIT);
    switch(dna)
    {
        case 0: return 'A';
        case 1: return 'C';
        case 2: return 'G';
        case 3: return 'T';
    }
    return '\0';
}

// get the bp at pos
char get_bp_3bit(char* genome, unsigned int pos)
{
    unsigned int byte_pos = pos / (BYTE_SIZE / ENCODE_SIZE_3BIT);
    unsigned int byte_off = pos % (BYTE_SIZE / ENCODE_SIZE_3BIT);
    unsigned char dna = genome[byte_pos];
    dna = dna << (byte_off * ENCODE_SIZE_3BIT);
    dna = dna >> (BYTE_SIZE - ENCODE_SIZE_3BIT);
    switch(dna)
    {
        case 0: return '$';
        case 1: return 'A';
        case 2: return 'C';
        case 3: return 'G';
        case 4: return 'T';
    }
    return '\0';
}

// write a bp at a position
void write_bp_2bit(char* genome, unsigned int pos, char val)
{
    unsigned int byte_pos = pos / (BYTE_SIZE / ENCODE_SIZE_2BIT);
    unsigned int byte_off = pos % (BYTE_SIZE / ENCODE_SIZE_2BIT) + 1;
    switch(val)
    {
        case 'A':
        {
            char mask = 0 << (BYTE_SIZE - byte_off * ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'C':
        {
            char mask = 1 << (BYTE_SIZE - byte_off * ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'G':
        {
            char mask = 2 << (BYTE_SIZE - byte_off * ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'T':
        {
            char mask = 3 << (BYTE_SIZE - byte_off * ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
    }
}

// write a bp at a position
void write_bp_3bit(char* genome, unsigned int pos, char val)
{
    unsigned int byte_pos = pos / (BYTE_SIZE / ENCODE_SIZE_3BIT);
    unsigned int byte_off = pos % (BYTE_SIZE / ENCODE_SIZE_3BIT) + 1;
    switch(val)
    {
        case '$':
        {
            char mask = 0 << (BYTE_SIZE - byte_off * ENCODE_SIZE_3BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'A':
        {
            char mask = 1 << (BYTE_SIZE - byte_off * ENCODE_SIZE_3BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'C':
        {
            char mask = 2 << (BYTE_SIZE - byte_off * ENCODE_SIZE_3BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'G':
        {
            char mask = 3 << (BYTE_SIZE - byte_off * ENCODE_SIZE_3BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'T':
        {
            char mask = 4 << (BYTE_SIZE - byte_off * ENCODE_SIZE_3BIT);
            genome[byte_pos] |= mask;
            break;
        }
    }
}

// read the file named file_name into the memory
void read_file(char* file_name, char** genome)
{
    FILE* file = fopen(file_name, "r");
    fseek(file, SEEK_SET, SEEK_END);
    unsigned int len = ftell(file);
    rewind(file);
    (*genome) = (char*) malloc(len);
    if(fread(*genome, sizeof(char), len, file) != len)
    {
       fprintf(stderr, "failed to load file\n");
       exit(1);
    }
    fclose(file);
}

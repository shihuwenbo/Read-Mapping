#include "misc.h"
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

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
    unsigned long long int bit_pos = pos * ENCODE_SIZE_2BIT;
    unsigned long long int byte_pos = bit_pos / BYTE_SIZE;
    unsigned long long int byte_off = bit_pos - byte_pos * BYTE_SIZE;
    unsigned char dna = genome[byte_pos];
    dna = dna << byte_off;
    dna = dna >> (BYTE_SIZE - ENCODE_SIZE_2BIT);
    switch(dna)
    {
        case 0: return 'a';
        case 1: return 'c';
        case 2: return 'g';
        case 3: return 't';
    }
    return '*';
}

// get the bp at pos
char get_bp_3bit(char* genome, unsigned int pos)
{
    unsigned long long int bit_pos = pos * ENCODE_SIZE_3BIT;
    unsigned long long int byte_pos = bit_pos / BYTE_SIZE;
    unsigned long long int byte_off = bit_pos - byte_pos * BYTE_SIZE;
    unsigned char dna;
    unsigned char bp0 = genome[byte_pos];
    unsigned char bp1 = genome[byte_pos+1];
    if(byte_off <= 5)
    {
        dna = bp0 << byte_off;
        dna = dna >> (BYTE_SIZE - ENCODE_SIZE_3BIT);
    }
    else
    {
        dna = bp0 << byte_off;
        dna = dna >> byte_off;
        dna <<= byte_off - (BYTE_SIZE - ENCODE_SIZE_3BIT);
        dna |= bp1 >> (2 * BYTE_SIZE - ENCODE_SIZE_3BIT - byte_off);
    }
    switch(dna)
    {
        case 0: return '$';
        case 1: return 'a';
        case 2: return 'c';
        case 3: return 'g';
        case 4: return 't';
    }
    return '*';
}

// write a bp at a position
void write_bp_2bit(char* genome, unsigned int pos, char val)
{
    unsigned long long int bit_pos = pos * ENCODE_SIZE_2BIT;
    unsigned long long int byte_pos = bit_pos / BYTE_SIZE;
    unsigned long long int byte_off = bit_pos - byte_pos * BYTE_SIZE;
    switch(val)
    {
        case 'a':
        {
            char mask = 0 << (BYTE_SIZE - byte_off - ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'c':
        {
            char mask = 1 << (BYTE_SIZE - byte_off - ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 'g':
        {
            char mask = 2 << (BYTE_SIZE - byte_off - ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
        case 't':
        {
            char mask = 3 << (BYTE_SIZE - byte_off - ENCODE_SIZE_2BIT);
            genome[byte_pos] |= mask;
            break;
        }
    }

}

// write a bp at a position
void write_bp_3bit(char* genome, unsigned int pos, char val)
{
    unsigned long long int bit_pos = pos * ENCODE_SIZE_3BIT;
    unsigned long long int byte_pos = bit_pos / BYTE_SIZE;
    unsigned long long int byte_off = bit_pos - byte_pos * BYTE_SIZE;
    unsigned char mask = 0, next_mask = 0;
    int shift_size = 0;
    if(byte_off <= 5)
    {
        shift_size = BYTE_SIZE - byte_off - ENCODE_SIZE_3BIT;
        switch(val)
        {
            case '$': mask = 0 << shift_size; break;
            case 'a': mask = 1 << shift_size; break;
            case 'c': mask = 2 << shift_size; break;
            case 'g': mask = 3 << shift_size; break;
            case 't': mask = 4 << shift_size; break;
        }
        genome[byte_pos] |= mask;
    }
    else
    {
        switch(val)
        {
            case '$':
                mask = 0 >> (byte_off - (BYTE_SIZE - ENCODE_SIZE_3BIT));
                next_mask = 0 << (2*BYTE_SIZE-ENCODE_SIZE_3BIT-byte_off);
                break;
            case 'a':
                mask = 1 >> (byte_off - (BYTE_SIZE - ENCODE_SIZE_3BIT));
                next_mask = 1 << (2*BYTE_SIZE-ENCODE_SIZE_3BIT-byte_off);
                break;
            case 'c':
                mask = 2 >> (byte_off - (BYTE_SIZE - ENCODE_SIZE_3BIT));
                next_mask = 2 << (2*BYTE_SIZE-ENCODE_SIZE_3BIT-byte_off);
                break;
            case 'g':
                mask = 3 >> (byte_off - (BYTE_SIZE - ENCODE_SIZE_3BIT));
                next_mask = 3 << (2*BYTE_SIZE-ENCODE_SIZE_3BIT-byte_off);
                break;
            case 't':
                mask = 4 >> (byte_off - (BYTE_SIZE - ENCODE_SIZE_3BIT));
                next_mask = 4 << (2*BYTE_SIZE-ENCODE_SIZE_3BIT-byte_off);
                break;
        }
        genome[byte_pos] |= mask;
        genome[byte_pos + 1] |= next_mask;
    }
}

// read the file named file_name into the memory
size_t read_file(char* file_name, char** genome)
{
    FILE* file = fopen(file_name, "r");
    fseek(file, SEEK_SET, SEEK_END);
    size_t len = ftell(file);
    rewind(file);
    (*genome) = (char*) malloc(len);
    if(fread(*genome, sizeof(char), len, file) != len)
    {
       fprintf(stderr, "failed to load file\n");
       exit(1);
    }
    fclose(file);
    return (size_t) len;
}

// write memory to disk saved in file_name
void write_file(char* file_name, char* genome, size_t file_size)
{
    FILE* genome_file = fopen(file_name, "wb");
    fwrite(genome, sizeof(char), file_size, genome_file);
    fclose(genome_file);
}

// create the data structure that supports O(1) select operation
void create_select_table(char* bitvect, unsigned int len, char** dir1,
        char** dir2, char** dir3)
{
    // initialization
    *dir1 = NULL;
    *dir2 = NULL;
    *dir3 = NULL;

    // block size for first level directory
    unsigned int blksz = (unsigned int) (ceil(log((double)len)/log(2.0)) *
                            ceil(log(log((double)len)/log(2.0))/log(2.0)));

    // initialize first directory
    unsigned int nent1 = (unsigned int) floor(((double)len)/((double)blksz));
    unsigned int entsz1 = (unsigned int) (ceil(log((double)len)/log(2.0)));
    unsigned int dirsz1 = (nent1*entsz1-1)/BYTE_SIZE + 1;
    (*dir1) = (char*) malloc(dirsz1*sizeof(char));
    for(unsigned int i = 0; i < nent1; i++)
        write_int(*dir1, entsz1, i, 0);
    
    // generate the first directory
    int bitcnt = 0, diriter = 0;
    for(unsigned int i = 0; i < len; i++)
    {
        if(get_bit(bitvect, i) == 1)
        {
            bitcnt++;
            if(bitcnt % blksz == 0)
            {
                printf("i: %d\n", i);
                write_int(*dir1, entsz1, diriter, i);
                diriter++;
            }
        }
    }

    // check if need to generate second directory

}

// return the integer at a position
unsigned int get_int(char* intary, unsigned int intsz, unsigned int pos)
{
    unsigned int rt = 0;
    unsigned int mask = 0;
    unsigned int bit_pos = intsz * pos;
    for(unsigned int i = 0; i < intsz; i++)
    {
        int bit = get_bit(intary, bit_pos+i);
        if(bit == 1)
        {
            mask = 1 << (intsz - i - 1);
            rt |= mask;
        }
        else
        {
            mask = ~ 1 << (intsz - i - 1);
            rt &= mask;
        }
    }
    return rt;
}

// write a integer at a position
void write_int(char* intary, unsigned int intsz,
                unsigned int pos, unsigned int val)
{
    unsigned int bit_pos = (pos + 1) * intsz - 1;
    unsigned int tmp_val = 0;
    for(unsigned int i = 0; i < intsz; i++)
    {
        tmp_val = val;
        tmp_val = tmp_val << (INT_SIZE - i - 1);
        tmp_val = tmp_val >> (INT_SIZE - 1);
        if(tmp_val == 1)
            set_bit(intary, bit_pos - i);
        else
            clear_bit(intary, bit_pos - i);
    }
}

// compute bwt based on the original file and the sa
char* compute_bwt_3bit(char* genome, unsigned int* sa,
     unsigned int genome_size, unsigned int file_size)
{
    char* bwt = (char*) malloc(file_size*sizeof(char));
    memset(bwt, 0, file_size*sizeof(char));
    for(unsigned int i = 0; i < genome_size; i++)
    {
        unsigned int pos = (unsigned int)
                        ((((unsigned long long int) (sa[i])) +
                        ((unsigned long long int) (genome_size-1))) %
                        ((unsigned long long int) genome_size));
        char bp = get_bp_3bit(genome, pos);
        write_bp_3bit(bwt, i, bp);
    }
    return bwt;
}

// compute partial suffix array
unsigned int* compute_partial_sa(unsigned int* sa,
        unsigned int genome_size, unsigned int sample_size)
{
    unsigned int *sa_pt = (unsigned int*)
                malloc((genome_size/sample_size)*sizeof(unsigned int));
    for(unsigned int i = 0, j = 0; i < genome_size; i++)
    {
        if(i % sample_size == 0)
        {
            sa_pt[i] = sa[i];
            j++;
        }
    }
    return sa_pt;
}

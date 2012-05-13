#include "suffix_sort.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/
// struct for naive suffix sort algorithm
typedef struct str_int
{
    char* str;
    int idx;
}
str_int_t;

// comparator for qsort used in suffx sorting algorithm
int suffix_compare(const void* lhs, const void* rhs)
{
    str_int_t* lhs_sit = (str_int_t*) lhs;
    str_int_t* rhs_sit = (str_int_t*) rhs;
    return strcmp(lhs_sit->str, rhs_sit->str);
}

// naive suffix sorting algorithm
int* ss_naive(char* str, int len)
{
    // generate the suffixes
    str_int_t* suffixes = (str_int_t*) malloc(len*sizeof(str_int_t));
    for(int i = 0; i < len; i++)
    {
        suffixes[i].str = str++;
        suffixes[i].idx = i;
    }

    // sort the suffixes
    qsort(suffixes, len, sizeof(str_int_t), suffix_compare);

    // generate the suffix array
    int* sa = (int*) malloc(len*sizeof(int));
    for(int i = 0; i < len; i++)
        sa[i] = suffixes[i].idx;

    // return the result
    return sa;
}

/*---------------------------------------------------------------------------*/
// helper function, given a letter, return its rank in the alphabet
int alpha_rank(char l)
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

// refined version of ss_mm
int* ss_mm(char* str, int len)
{

    /* allocating memory for arrays required for sorting */

    int* pos = (int*) malloc(len * sizeof(int));
    int* prm = (int*) malloc(len * sizeof(int));
    int* count = (int*) malloc(len * sizeof(int));
    int bit_vect_sz = (len - 1) / 8 + 1;
    char* bh = (char*) malloc(bit_vect_sz * sizeof(char));
    char* b2h = (char*) malloc(bit_vect_sz * sizeof(char));


    /* first stage: radix sorting according to the first letter */

    // initialize the bin for counting sort
    int bin[ALPHA_SIZE + 1];
    for(int i = 0; i < ALPHA_SIZE + 1; i++)
        bin[i] = 0;

    // count the number of each letter
    for(int i = 0; i < len; i++)
        bin[alpha_rank(str[i])]++;

    // compute offset of each letter
    int offset[ALPHA_SIZE + 1];
    offset[0] = 0;
    for(int i = 1; i < ALPHA_SIZE + 1; i++)
        offset[i] = offset[i-1] + bin[i-1];

    // use the offset to compute the first stage suffix array
    for(int i = 0; i < len; i++)
        pos[offset[alpha_rank(str[i])]++] = i;

    // compute the first stage prm
    for(int i = 0; i < len; i++)
        prm[pos[i]] = i;

    // compute the first stage bh
    set_bit(bh, 0);
    for(int i = 1; i < len; i++)
    {
        if(str[pos[i]] != str[pos[i-1]])
            set_bit(bh, i);
        else
            clear_bit(bh, i);
    }

    /* inductive stages: sort pos incrementally in n*log(n) time */

    for(int H = 1; H < len; H <<= 1)
    {

        // printf("=====================H: %d ====================\n", H);

    /* reset prm such that prm[i] points to the left most bucket */

        // find how many bins
        int num_bin = 0;
        for(int i = 0; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
                num_bin++;
        }

        // find the left most position of each bin
        int* left_most = (int*) malloc(num_bin*sizeof(int));
        for(int i = 0, j = 0; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
            {
                left_most[j] = i;
                j++;
            }
        }

        // set prm[i] to point to the left most in each bin
        for(int i = 0, j = -1; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
                j++;
            prm[pos[i]] = left_most[j];
        }

        // initialize count
        for(int i = 0; i < len; i++)
            count[i] = 0;

        // initialize b2h
        for(int i = 0; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
                set_bit(b2h, i);
            else
                clear_bit(b2h, i);
        }

        /*
        // print out information for debug
        printf("********************BEFORE********************\n");
        printf("i\tbh\tbh2\tprm\tpos\n");
        for(int i = 0; i < len; i++)
        {
            printf("%d\t%d\t%d\t%d\t%d = %s\n",
                    i,get_bit(bh,i),get_bit(b2h,i),prm[i],pos[i],str+pos[i]);
            if(i < len-1 && get_bit(bh,i+1) == 1)
                printf("----------------------------------------------\n");
        }
        */

        // scan bucket
        int l = 0, r = 0, n = 0;
        while(l < len)
        {
            // find left and right boundary of a bucket
            for(r = l + 1; r < len; r++)
            {
                if(get_bit(bh, r) == 1)
                {
                    r = r -1;
                    break;
                }
            }
            if(r >= len)
                r = r - 1;

            // increment count, set prm, set b2h
            for(int i = l; i <= r; i++)
            {
                int Ti = pos[i] - H;
                if(Ti >= 0)
                {
                    count[prm[Ti]]++;
                    prm[Ti] += count[prm[Ti]] - 1;
                    set_bit(b2h, prm[Ti]);
                }
            }

            // update b2h
            for(int i = l; i <= r; i++)
            {
                int Ti = pos[i] - H;
                if(Ti >= 0)
                {
                    if(get_bit(b2h, prm[Ti]) == 1)
                    {
                        int j;
                        for(j = prm[Ti] + 1;
                            get_bit(bh,j)==0 && get_bit(b2h,j)==1; j++)
                            continue;
                        for(int k = prm[Ti] + 1; k < j; k++)
                            clear_bit(b2h, k);
                    }
                }
            }

            // update left side boundary
            l = r + 1;
            n++;
        }

        // update pos
        for(int i = 0; i < len; i++)
            pos[prm[i]] = i;

        // copy b2h to bh
        for(int i = 0; i < bit_vect_sz; i++)
            bh[i] = b2h[i];

        // free temp dynamically allocated memory
        free(left_most);

        /*
        // print out information for debug
        printf("********************AFTER*********************\n");
        printf("i\tbh\tbh2\tprm\tpos\n");
        for(int i = 0; i < len; i++)
        {
            printf("%d\t%d\t%d\t%d\t%d = %s\n",
                    i,get_bit(bh,i),get_bit(b2h,i),prm[i],pos[i],str+pos[i]);
            if(i < len-1 && get_bit(bh,i+1) == 1)
                printf("----------------------------------------------\n");
        }
        */
    }

    /* free dynamically allocated memory */

    free(prm);
    free(bh);
    free(b2h);
    free(count);

    return pos;
}

/*---------------------------------------------------------------------------*/
// get the pos-th bit in the array bit_vector
int get_bit(char* bit_vector, int pos)
{
    int idx = pos / 8;
    int boff = pos % 8;
    unsigned char byte = bit_vector[idx];
    byte = byte << boff;
    byte = byte >> 7;
    return byte;
}

// set the pos-th bit in the bit_vector to 1
void set_bit(char* bit_vector, int pos)
{
    int idx = pos / 8;
    int boff = pos % 8;
    unsigned char mask = 1 << (8 - boff -1);
    bit_vector[idx] = bit_vector[idx] | mask;
}

// clear the pos-th bit in the bit_vector to 0
void clear_bit(char* bit_vector, int pos)
{
    int idx = pos / 8;
    int boff = pos % 8;
    unsigned char mask = ~ (1 << (8 - boff -1));
    bit_vector[idx] = bit_vector[idx] & mask;
}

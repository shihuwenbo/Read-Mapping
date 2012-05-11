#include "suffix_sort.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

// suffix sorting algorithm by manber and myers
int* suffix_sorting_2(char* str, int len)
{
/* allocating memory for arrays required for sorting */

    int* pos = (int*) malloc(len * sizeof(int));
    int* prm = (int*) malloc(len * sizeof(int));
    int* bh = (int*) malloc(len * sizeof(int));
    int* b2h = (int*) malloc(len * sizeof(int));
    int* count = (int*) malloc(len * sizeof(int));

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
    bh[0] = 1;
    for(int i = 1; i < len; i++)
        bh[i] = str[pos[i]] != str[pos[i-1]] ? 1 : 0;

/* inductive stages: sort pos incrementally in n*log(n) time */

    for(int H = 2; H < len; H <<= 1)
    {
        // reset prm such that prm[i] points to the left most bucket
        int least[ALPHA_SIZE + 1];
        for(int i = 0; i < ALPHA_SIZE + 1; i++)
            least[i] = 2147483647u;
        for(int i = 0; i < len; i++)
        {
            if(prm[i] < least[alpha_rank(str[i])])
                least[alpha_rank(str[i])] = prm[i];
        }
        for(int i = 0; i < len; i++)
            prm[i] = least[alpha_rank(str[i])];

        // initialize count
        for(int i = 0; i < len; i++)
            count[i] = 0;

        // scan bucket
        int l = 0, r = 0;
        while(l < len)
        {
            // find left and right boundary of a bucket
            for(r = l + 1; r < len; r++)
            {
                if(bh[r] == 1)
                {
                    r = r -1;
                    break;
                }
            }
            if(r >= len)
                r = r - 1;

            // initialize b2h
            for(int i = 0; i < len; i++)
                b2h[i] = 0;

            // increment count, set prm, set b2h
            for(int i = l; i <= r; i++)
            {
                int Ti = pos[i] - H;
                if(Ti >= 0)
                {
                    count[prm[Ti]]++;
                    prm[Ti] = prm[Ti] + count[prm[Ti]] - 1;
                    b2h[prm[Ti]] = 1;
                }
            }

            for(int i = 0; i < len; i++)
                printf("%d ", prm[i]);
            printf("\n");
            // update b2h
            for(int i = 0; i < ALPHA_SIZE + 1; i++)
                least[i] = 2147483647u;
            for(int i = 0; i < len; i++)
            {
                if(prm[i] < least[alpha_rank(str[i])])
                    least[alpha_rank(str[i])] = prm[i];
            }
            for(int i = 0; i < len; i++)
            {
                int find = 0;
                for(int j = 0; j < ALPHA_SIZE + 1; j++)
                {
                    if(least[j] == i)
                    {
                        find = 1;
                        break;
                    }
                }
                if(find == 1)
                    b2h[i] = 1;
                else
                    b2h[i] = 0;
            }

            // update left side boundary
            l = r + 1;
        }
        
        // update pos and set bh
        for(int i = 0; i < len; i++)
            pos[prm[i]] = i;
        for(int i = 0; i < len; i++)
            bh[i] = b2h[i];
    }

/* free dynamically allocated memory */

    free(prm);
    free(bh);
    free(b2h);
    free(count);

/* return suffix array */

    return pos;
}

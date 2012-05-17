#include "misc.h"
#include "suffix_sort.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// generate compresses suffix array
void gen_csa(char* str, unsigned int len)
{
    /* compute segment size */
    double lseg = ceil(((double)len) / (log(len)/log(2)));
    unsigned int nseg = (unsigned int) ceil((double) (len) / lseg);

    /* compute the suffix array of the last segment */
    int lst_seg_pos = (nseg-1) * lseg;
    int lst_seg_len = len - lst_seg_pos;

    /* compute the first suffix array and the inverse of suffix array */
    char* lst_seg = str + lst_seg_pos;
    int* lst_sa = ss_mm_8bit(lst_seg, lst_seg_len);
    int* lst_sa_inv = (int*) malloc(lst_seg_len*sizeof(int));
    for(int i = 0; i < lst_seg_len; i++)
        lst_sa_inv[lst_sa[i]] = i;

    /* compute psi */
    int* psi = (int*) malloc(lst_seg_len*sizeof(int));
    psi[0] = lst_sa_inv[0];
    for(int i = 1; i < lst_seg_len; i++)
        psi[i] = lst_sa_inv[lst_sa[i]+1];

    /* compute ac and nc for each letter in the alphabet */
    int ac[ALPHA_SIZE + 1];
    int nc[ALPHA_SIZE + 1];
    memset(nc, 0, (ALPHA_SIZE+1)*sizeof(int));
    memset(ac, 0, (ALPHA_SIZE+1)*sizeof(int));
    char* alphabet = "$acgt";
    for(int i = 0; i < lst_seg_len; i++)
    {
        for(int j = 0; j < ALPHA_SIZE + 1; j++)
        {
            if(lst_seg[i] < alphabet[j])
                ac[j]++;
            if(lst_seg[i] == alphabet[j])
                nc[j]++;
        }
    }

    /* verify theory: psi should be an increasing sequence */
    int* covered = (int*) malloc(lst_seg_len*sizeof(int));
    memset(covered, 0, lst_seg_len*sizeof(int));
    for(int i = 0; i < ALPHA_SIZE + 1; i++)
    {
        int start = ac[i], end = ac[i] + nc[i];
        covered[psi[start]] = 1;
        for(int j = start + 1; j < end; j++)
        {
            covered[psi[j]] = 1;
            if(psi[j-1] > psi[j])
                printf("psi[%d] < psi[%d]\n",j,j-1);
        }
    }

    /* verify theory: psi should cover all indexes */
    for(int i = 0; i < lst_seg_len; i++)
    {
        if(covered[i] == 0)
            printf("%d: missing\n", i);
    }

    /* free memory */
    free(lst_sa);
    free(lst_sa_inv);
    free(psi);
    free(covered);
}

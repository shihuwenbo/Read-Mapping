#include "misc.h"
#include "suffix_sort.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

    /* print out psi for debug */
    /*
    for(int i = 0; i < lst_seg_len; i++)
        printf("%d ", psi[i]);
    printf("\n");
    */

    /* free memory */
    free(lst_sa);
    free(lst_sa_inv);
}

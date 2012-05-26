#include "misc.h"
#include "search.h"
#include <stdio.h>

// given a file of short reads, search it in genome, store result in all_ans
// maximum result is limited by ans_size
// maximum mismatch is kerr
void search(char* bwt, unsigned int genome_size, char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ, unsigned int spsize, unsigned int* all_ans,
        unsigned int ans_size, unsigned int kerr)
{
    const char alphabet[] = "acgt";
    for(unsigned int i = 0; i < read_num; i++)
    {
        unsigned int* ans_ptr = all_ans + i * ans_size;
        unsigned int lst = (i + 1)* read_size -1;
        kmismatch(sr,bwt,sml,occ,psa,kerr,lst,0,genome_size-1,ans_ptr,
            0,ans_size,genome_size,spsize,alphabet);
    }
}

// given a bwt and a partial occ array, find the actual occ value
unsigned int get_occ(char* bwt, unsigned int* occ, unsigned int genome_size,
        unsigned int sample_size, long long int pos, char alpha)
{
    if(pos < 0)
        return 0;

    unsigned int occ_off = pos / sample_size;
    unsigned int* base_ptr = &occ[occ_off*ALPHA_SIZE];

    unsigned int occi = 0;
    switch(alpha)
    {
        case 'a': occi = base_ptr[0]; break;
        case 'c': occi = base_ptr[1]; break;
        case 'g': occi = base_ptr[2]; break;
        case 't': occi = base_ptr[3]; break;
    }

    unsigned int ext_cnt = 0;
    for(unsigned int i = occ_off * sample_size + 1;
        i <= pos && i < genome_size; i++)
    {
        char bp = get_bp_3bit(bwt, i);
        if(bp == alpha)
            ext_cnt++;
    }

    occi += ext_cnt;
    return occi;
}

// k-mismatch search
unsigned int kmismatch(char* sr, char* bwt, unsigned int* sml,
        unsigned int* occ, unsigned int* psa, long long int kerr,
        unsigned int lst, unsigned int sp, unsigned int ep, unsigned int* ans,
        unsigned int ans_cnt, unsigned int ans_size,
        unsigned int genome_size, unsigned int sample_size,
        const char *alphabet)
{
    if(sp > ep || ans_cnt >= ans_size)
        return 0;
    
    if(lst == 0)
    {
        unsigned int cnt = 0;
        for(unsigned int i = ans_cnt, j = sp;
            i < ans_size && j <= ep; i++, j++)
        {
            ans[i] = psa[j];
            cnt++;
        }
        return cnt;
    }

    unsigned sp2 = 0, ep2 = 0, kerr2 = 0, new_ans_cnt = ans_cnt;
    
    for(unsigned int i = 0; i < ALPHA_SIZE; i++)
    {
        sp2 = sml[alpha_rank(alphabet[i])] + 1 +
            get_occ(bwt,occ,genome_size,sample_size,(long long int)sp-1,alphabet[i]);
        ep2 = sml[alpha_rank(alphabet[i])] +
            get_occ(bwt,occ,genome_size,sample_size,ep,alphabet[i]);
        if(get_bp_2bit(sr, lst) != alphabet[i])
            kerr2 = kerr - 1;
        else
            kerr2 = kerr;
        if(kerr2 >= 0)
        {
            new_ans_cnt += kmismatch(sr,bwt,sml,occ,psa,kerr2,
                             lst-1,sp2,ep2,ans,new_ans_cnt,ans_size,genome_size,
                             sample_size,alphabet);
        }
    }

    return new_ans_cnt;
}

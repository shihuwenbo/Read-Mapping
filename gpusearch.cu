#include "misc.h"
#include "gpusearch.h"
#include "cuda.h"
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

static cudaError_t crc;

int nblock_size = 64;
int maxgsx = 65535;

// given a letter, return its rank in the alphabet
__device__
unsigned int alpha_rank(char l)
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
    return 5;
}

// get the bp at pos
__device__
char gpu_get_bp_2bit(char* genome, unsigned int pos)
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

// given a bwt and a partial occ array, find the actual occ value
__device__
unsigned int gpu_get_occ(char* bwt, unsigned int* occ, unsigned int genome_size,
        unsigned int sample_size, long long int pos, char alpha)
{
    if(pos < 0)
        return 0;

    unsigned int occ_off = pos / sample_size;
    unsigned int* base_ptr = &occ[occ_off*ALPHA_SIZE];

    long long int occi = 0;
    switch(alpha)
    {
        case 'a': occi = base_ptr[0]; break;
        case 'c': occi = base_ptr[1]; break;
        case 'g': occi = base_ptr[2]; break;
        case 't': occi = base_ptr[3]; break;
    }

    unsigned int ext_cnt = 0;
    for(unsigned int i = occ_off * sample_size + 1;
        i < pos && i < genome_size; i++)
    {
        char bp = '*';
        if(i < BWT_DPOS)
            bp = gpu_get_bp_2bit(bwt, i);
        else if(i == BWT_DPOS)
            bp = '$';
        else
            bp = gpu_get_bp_2bit(bwt, i-1);
        if(bp == alpha)
            ext_cnt++;
    }

    occi += ext_cnt;

    if(pos % sample_size == 0)
    {
        char bp = '*';
        if(pos < BWT_DPOS)
            bp = gpu_get_bp_2bit(bwt, pos);
        else if(pos == BWT_DPOS)
            bp = '$';
        else
            bp = gpu_get_bp_2bit(bwt, pos-1);
        if(bp == alpha)
            occi--;
        if(occi < 0)
            occi = 0;
    }
    return (unsigned int)occi;
}

// get suffix array value
__device__
unsigned int gpu_get_sa_val(char* bwt, unsigned int*sml, unsigned int* occ,
        unsigned int*psa, unsigned int genome_size, unsigned int sample_size,
        unsigned int pos)
{
    unsigned int nmov = 0;
    int lucky = 0;
    while(pos != BWT_DPOS)
    {
        if(pos % sample_size == 0)
        {
            lucky = 1;
            break;
        }
        char bp = '*';
        if(pos < BWT_DPOS)
            bp = gpu_get_bp_2bit(bwt, pos);
        else if(pos == BWT_DPOS)
            bp = '$';
        else
            bp = gpu_get_bp_2bit(bwt, pos-1);
        pos = sml[alpha_rank(bp)] + 
            gpu_get_occ(bwt,occ,genome_size,sample_size,(long long int)pos,bp);
        nmov++;
    }
    if(lucky)
        nmov += psa[pos/sample_size];
    return nmov;
}

// k-mismatch search
__device__
unsigned int gpu_kmismatch(char* sr, char* bwt, unsigned int* sml,
        unsigned int* occ, unsigned int* psa, int kerr,
        unsigned int rs, unsigned int re, unsigned int sp, unsigned int ep,
        unsigned int* ans, unsigned int ans_cnt, unsigned int ans_size,
        unsigned int genome_size, unsigned int sample_size,
        const char *alphabet)
{
    /*
    for(long long int i = re; i >= (long long int)rs && ep >= sp; i--)
    {
        char bp = gpu_get_bp_2bit(sr, i);
        sp = sml[alpha_rank(bp)] + 1 +
             gpu_get_occ(bwt,occ,genome_size,sample_size,(long long int)sp-1,bp);
        ep = sml[alpha_rank(bp)] +
             gpu_get_occ(bwt,occ,genome_size,sample_size,(long long int)ep,bp);
    }
    if(sp <= ep)
        ans[0] = psa[sp-1];
    return 0;
    */
    if(sp > ep && ans_cnt < ans_size)
        return 0;
    if(re <= rs)
    {
        unsigned int cnt = 0;
        for(unsigned int i = sp - 1, j = ans_cnt; 
            i <= ep - 1 && j < ans_size; i++, j++)
        {
            ans[j] = gpu_get_sa_val(bwt,sml,occ,psa,genome_size,sample_size,i)-1;
            cnt++;
        }
        return cnt;
    }
    unsigned int new_ans_cnt = ans_cnt;
    for(unsigned int i = 0; i < ALPHA_SIZE; i++)
    {
        unsigned int sp2 = sml[alpha_rank(alphabet[i])] + 1 +
            gpu_get_occ(bwt,occ,genome_size,sample_size,
                    (long long int)sp-1,alphabet[i]);
        unsigned int ep2 = sml[alpha_rank(alphabet[i])] +
            gpu_get_occ(bwt,occ,genome_size,sample_size,
                    (long long int)ep,alphabet[i]);

        int kerr2 = 0;
        if(gpu_get_bp_2bit(sr, re) != alphabet[i])
            kerr2 = kerr - 1;
        else
            kerr2 = kerr;
        if(kerr2 >= 0)
        {
            new_ans_cnt += gpu_kmismatch(sr,bwt,sml,occ,psa,kerr2,rs,re-1,sp2,
                        ep2,ans,new_ans_cnt,ans_size,genome_size,
                        sample_size,alphabet);
        }
    }
    return new_ans_cnt;
}

// given a file of short reads, search it in genome, store result in all_ans
// maximum result is limited by ans_size
// maximum mismatch is kerr
__global__
void search_kernel(char* bwt, unsigned int genome_size, char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ, unsigned int spsize, unsigned int* all_ans,
        unsigned int ans_size, int kerr)
{
    const char alphabet[] = "acgt";
    unsigned int i = (blockIdx.y*gridDim.x+blockIdx.x)*blockDim.x+threadIdx.x;
    if(i < read_num)
    {
        unsigned int* ans_ptr = all_ans + i * ans_size;
        unsigned int rs = i*read_size;
        unsigned int re = (i+1)*read_size-1;
        gpu_kmismatch(sr,bwt,sml,occ,psa,kerr,rs,re,0,genome_size-1,ans_ptr,
            0,ans_size,genome_size,spsize,alphabet);
    }
}

// c interface
extern "C++" void gpu_search(char* bwt, unsigned int genome_size, char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ, unsigned int spsize, unsigned int* all_ans,
        unsigned int ans_size, int kerr)
{
    unsigned int x_size, y_size;
    dim3 dimBlock(nblock_size);
    x_size = (read_num - 1)/nblock_size + 1; 
    y_size = (x_size - 1)/maxgsx + 1;  
    x_size = x_size < maxgsx ? x_size : maxgsx;
    dim3 dimGrid(x_size, y_size);
    crc = cudaGetLastError();
    search_kernel<<<dimGrid, dimBlock>>>(bwt,genome_size,sr,psa,read_num,read_size,
         sml,occ,spsize,all_ans,ans_size,kerr);
    cudaThreadSynchronize();
    crc = cudaGetLastError();
    if (crc)
    {
        printf("gpusearch error=%d:%s\n",crc,cudaGetErrorString(crc));
        exit(1);
    }   
}

// functions for memory copy allocation and deallocation
extern "C++" void gmalloc(char** gptr, size_t size)
{
    cudaMalloc(gptr, size);
    return;
}

extern "C++" void gmemcpy_htod(char* dst, char* src, size_t size)
{
   cudaMemcpy(dst, src, size, cudaMemcpyHostToDevice);
   return;
}

extern "C++" void gmemcpy_dtoh(char* dst, char* src, size_t size)
{
   cudaMemcpy(dst, src, size, cudaMemcpyDeviceToHost);
   return;
}

extern "C++" void gfree(char* src)
{
   cudaFree(src);
   return;
}

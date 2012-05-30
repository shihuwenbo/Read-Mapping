
#include "misc.h"
#include "gpusearch.h"
#include "cuda.h"
#include <cuda_runtime.h>
#include <stdio.h>
#include <stdlib.h>

static cudaError_t crc;
int nblock_size = 512;
int maxgsx = 65535;

__device__ char *kmbwt, *kmsr;
__device__ unsigned int *kmocc, *kmpsa;
__device__ unsigned int *kmsml;

// given a letter, return its rank in the alphabet
__device__ inline
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
__device__ inline
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

// write a bp at a position
__device__ inline
void gpu_write_bp_2bit(char* genome, unsigned int pos, char val)
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

// given a bwt and a partial occ array, find the actual occ value
__device__ inline
unsigned int gpu_get_occ(long long int pos, char alpha)
{
    if(pos < 0)
        return 0;

    unsigned int occ_off = pos / SAMPLE_SIZE_TEST;

    long long int occi = 0;
    switch(alpha)
    {
        case 'a': occi = kmocc[occ_off*ALPHA_SIZE+0]; break;
        case 'c': occi = kmocc[occ_off*ALPHA_SIZE+1]; break;
        case 'g': occi = kmocc[occ_off*ALPHA_SIZE+2]; break;
        case 't': occi = kmocc[occ_off*ALPHA_SIZE+3]; break;
    }

    unsigned int ext_cnt = 0;
    for(unsigned int i = occ_off * SAMPLE_SIZE_TEST + 1;
        i < pos && i < (unsigned int)GENOME_SIZE_TEST; i++)
    {
        char bp = '*';
        if(i < BWT_DPOS)
            bp = gpu_get_bp_2bit(kmbwt, i);
        else if(i == BWT_DPOS)
            bp = '$';
        else
            bp = gpu_get_bp_2bit(kmbwt, i-1);
        if(bp == alpha)
            ext_cnt++;
    }

    occi += ext_cnt;

    if(pos % SAMPLE_SIZE_TEST == 0)
    {
        char bp = '*';
        if(pos < BWT_DPOS)
            bp = gpu_get_bp_2bit(kmbwt, pos);
        else if(pos == BWT_DPOS)
            bp = '$';
        else
            bp = gpu_get_bp_2bit(kmbwt, pos-1);
        if(bp == alpha)
            occi--;
        if(occi < 0)
            occi = 0;
    }
    return (unsigned int)occi;
}

// get suffix array value
__device__ inline
unsigned int gpu_get_sa_val(unsigned int pos)
{
    unsigned int nmov = 0;
    while(pos != BWT_DPOS)
    {
        if(pos % SAMPLE_SIZE_TEST == 0)
        {
            nmov += kmpsa[pos/SAMPLE_SIZE_TEST];
            break;
        }
        char bp = '*';
        if(pos < BWT_DPOS)
            bp = gpu_get_bp_2bit(kmbwt, pos);
        else if(pos == BWT_DPOS)
            bp = '$';
        else
            bp = gpu_get_bp_2bit(kmbwt, pos-1);
        pos = kmsml[alpha_rank(bp)] + 
            gpu_get_occ((long long int)pos,bp);
        nmov++;
    }
    return nmov;
}

// k-mismatch search
__device__ inline
void gpu_kmismatch(int kerr,unsigned int re,
              unsigned int sp,unsigned int ep,unsigned int* ans)
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
    if(sp > ep)
        return;

    if(re % READ_SIZE == 0)
    {
        ans[0] = gpu_get_sa_val(sp-1)-1;
        return;
    }

    unsigned int sp2 = kmsml[alpha_rank('a')] + 1 +
        gpu_get_occ((long long int)sp-1,'a');
    unsigned int ep2 = kmsml[alpha_rank('a')] +
        gpu_get_occ((long long int)ep,'a');
    int kerr2 = 0;
    if(gpu_get_bp_2bit(kmsr, re) != 'a')
        kerr2 = kerr - 1;
    else
        kerr2 = kerr;
    if(kerr2 >= 0)
        gpu_kmismatch(kerr2,re-1,sp2,ep2,ans);

    sp2 = kmsml[alpha_rank('c')] + 1 +
        gpu_get_occ((long long int)sp-1,'c');
    ep2 = kmsml[alpha_rank('c')] +
        gpu_get_occ((long long int)ep,'c');
    if(gpu_get_bp_2bit(kmsr, re) != 'c')
        kerr2 = kerr - 1;
    else
        kerr2 = kerr;
    if(kerr2 >= 0)
        gpu_kmismatch(kerr2,re-1,sp2,ep2,ans);

    sp2 = kmsml[alpha_rank('g')] + 1 +
        gpu_get_occ((long long int)sp-1,'g');
    ep2 = kmsml[alpha_rank('g')] +
        gpu_get_occ((long long int)ep,'g');
    if(gpu_get_bp_2bit(kmsr, re) != 'g')
        kerr2 = kerr - 1;
    else
        kerr2 = kerr;
    if(kerr2 >= 0)
        gpu_kmismatch(kerr2,re-1,sp2,ep2,ans);

    sp2 = kmsml[alpha_rank('t')] + 1 +
        gpu_get_occ((long long int)sp-1,'t');
    ep2 = kmsml[alpha_rank('t')] +
        gpu_get_occ((long long int)ep,'t');
    if(gpu_get_bp_2bit(kmsr, re) != 't')
        kerr2 = kerr - 1;
    else
        kerr2 = kerr;
    if(kerr2 >= 0)
        gpu_kmismatch(kerr2,re-1,sp2,ep2,ans);

    return;
}

// given a file of short reads, search it in genome, store result in all_ans
// maximum result is limited by ans_size
// maximum mismatch is kerr
__global__
void search_kernel(char* bwt, char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ, unsigned int* all_ans, int kerr)
{
    unsigned int i = (blockIdx.y*gridDim.x+blockIdx.x)*blockDim.x+threadIdx.x;
    if(i < 100000)
    {
        kmbwt = bwt;
        kmocc = occ;
        kmpsa = psa;
        kmsr = sr; 
        kmsml = sml;
        unsigned int* ans_ptr = all_ans + i;
        unsigned int re = read_size*(i+1)-1;
        unsigned int sp = 0;
        unsigned int ep = (unsigned int)GENOME_SIZE_TEST - 1;
        gpu_kmismatch(kerr,re,sp,ep,ans_ptr);
    }
}

// c interface
extern "C++" void gpu_search(char* bwt, char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ, unsigned int* all_ans, int kerr)
{
    unsigned int x_size, y_size;
    dim3 dimBlock(nblock_size);
    x_size = (100000 - 1)/nblock_size + 1; 
    y_size = (x_size - 1)/maxgsx + 1;  
    x_size = x_size < maxgsx ? x_size : maxgsx;
    dim3 dimGrid(x_size, y_size);
    crc = cudaGetLastError();
    search_kernel<<<dimGrid, dimBlock>>>
      (bwt,sr,psa,read_num,read_size,sml,occ,all_ans,kerr);
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

extern "C++" void set_stack_size(size_t size)
{
   cudaThreadSetLimit(cudaLimitStackSize, size); 
   return;
}

extern "C++" void set_cache_size(int nscache)
{
   cudaFuncCache cpref;
   if ((nscache < 0) || (nscache > 2))
      return;
   if (nscache==0)
      cpref = cudaFuncCachePreferNone;
   else if (nscache==1)
      cpref = cudaFuncCachePreferShared;
   else if (nscache==2)
      cpref = cudaFuncCachePreferL1;
   cudaThreadSetCacheConfig(cpref);
   return;
}

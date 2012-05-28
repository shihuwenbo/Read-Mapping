#include <stdlib.h>

void gpu_search(char* bwt,char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ,unsigned int* all_ans,
        int kerr);

void gmalloc(char** gptr, size_t size);

void gmemcpy_htod(char* dst, char* src, size_t size);

void gmemcpy_dtoh(char* dst, char* src, size_t size);

void gfree(char* src);

void set_stack_size(size_t size);

void set_cache_size(int nscache);

#include <stdlib.h>

void gpu_search(char* bwt, unsigned int genome_size, char* sr, unsigned int* psa,
        unsigned int read_num, unsigned int read_size, unsigned int* sml,
        unsigned int* occ, unsigned int spsize, unsigned int* all_ans,
        unsigned int ans_size, int kerr);

void gmalloc(char** gptr, size_t size);

void gmemcpy_htod(char* dst, char* src, size_t size);

void gmemcpy_dtoh(char* dst, char* src, size_t size);

void gfree(char* src);

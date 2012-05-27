#include "search.h"
#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    // need: program name, bwt file
    // sml file, occ file, short read file
    if(argc != 7)
    {
        printf("Usage: [map] [BWT] [READ] [SML] [OCC] [PSA] [OPT]\n");
        return 0;
    }
    else
    {
        char *bwt, *sr, *smlc, *occc, *psac;
        read_file(argv[1], &bwt);
        read_file(argv[2], &sr);
        read_file(argv[3], &smlc);
        read_file(argv[4], &occc);
        read_file(argv[5], &psac);

        unsigned int* sml = (unsigned int*) smlc;
        unsigned int* occ = (unsigned int*) occc;
        unsigned int* psa = (unsigned int*) psac;

        unsigned int read_size = READ_SIZE;
        unsigned int genome_size = 30001, sample_size = 10, read_num = 100;
        if(argv[6][0] == 't')
        {
            genome_size = GENOME_SIZE;
            sample_size = SAMPLE_SIZE;
            read_num = READ_NUM;
        }

        unsigned int ans_size = 5;
        unsigned int* all_ans = (unsigned int*)
                        malloc(ans_size*read_num*sizeof(unsigned int));
        memset(all_ans, -1, ans_size*read_num*sizeof(unsigned int));
        int kerr = 2;
        search(bwt,genome_size,sr,psa,read_num,read_size,sml,occ,sample_size,
            all_ans,ans_size,kerr);
        
        for(unsigned int i = 0; i < read_num; i++)
        {
            printf("read #%u: ", i);
            unsigned int* ptr = &all_ans[i*ans_size];
            for(unsigned int j = 0; j < ans_size; j++)
            {
                if((int)ptr[j] != -1)
                {
                    if(j == ans_size - 1 || (int)ptr[j+1] == -1)
                        printf("%u", ptr[j]);
                    else
                        printf("%u, ", ptr[j]);
                }
                else
                    break;
            }
            printf("\n");
        }

        free(bwt);
        free(smlc);
        free(occc);
        free(sr);
        free(psac);
    }
}

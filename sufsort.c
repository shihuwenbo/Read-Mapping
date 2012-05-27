#include "misc.h"
#include "suffix_sort.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc == 1)
    {
        char input[] = "$gacgcgatc";
        unsigned int len = strlen(input);
        char* input_3bit = (char*) malloc(len*sizeof(int));
        for(unsigned int i = 0; i < len; i++)
            write_bp_3bit(input_3bit, i, input[i]);
        unsigned int* sa = ss_mm_3bit(input_3bit, len);
        int* sa8 = ss_naive_8bit(input, len);
        for(unsigned int i = 0; i < len; i++)
            printf("%d, ", sa8[i]);
        printf("\n");
        return 0;
        for(unsigned int i = 0; i < len; i++)
        {
            if((int)sa[i] != sa8[i])
            {
                printf("sa8: ");
                for(unsigned int j = 0; j < len; j++)
                    printf("%d ", sa8[j]);
                printf("\n");
                printf("sa: ");
                for(unsigned int j = 0; j < len; j++)
                    printf("%d ", sa[j]);
                printf("\n");
                return 0;
            }
        }
        char* bwt = compute_bwt_3bit(input_3bit, sa, len, len);
        for(unsigned int i = 0; i < len; i++)
        {
            char bp = get_bp_3bit(bwt, i);
            printf("%c", bp);
        }
        printf("\n");
        free(input_3bit);
        free(bwt);
        return 0;
    }
    else if(argc == 3)
    {
        // initialize parameter
        unsigned int genome_size = 30000;
        unsigned int sample_size = 10;
        if(argv[2][0] == 't' || argv[2][0] == 'T')
        {
            genome_size = GENOME_SIZE;
            sample_size = SAMPLE_SIZE;
        }

        // generate output file names
        unsigned int fnlen = strlen(argv[1]);
        char* psafn = (char*) malloc((fnlen+5)*sizeof(char));
        strcpy(psafn, argv[1]);
        psafn[fnlen] = '_'; psafn[fnlen+1] = 'p'; psafn[fnlen+2] = 's';
        psafn[fnlen+3] = 'a'; psafn[fnlen+4] = '\0';
        char *safn = (char*) malloc((fnlen+4)*sizeof(char));
        strcpy(safn, argv[1]);
        safn[fnlen] = '_'; safn[fnlen+1] = 's'; safn[fnlen+2] = 'a';
        safn[fnlen+3] = '\0';
        char *bwtfn = (char*) malloc((fnlen+5)*sizeof(char));
        strcpy(bwtfn, argv[1]);
        bwtfn[fnlen] = '_'; bwtfn[fnlen+1] = 'b'; bwtfn[fnlen+2] = 'w';
        bwtfn[fnlen+3] = 't'; bwtfn[fnlen+4] = '\0';

        // read input file
        char *genome_file;
        printf("started to read in file...\n");
        unsigned int gfsz = read_file(argv[1], &genome_file);
        printf("genome file read in...\n");

        // compute complete suffix array
        printf("started to generate suffix array...\n");
        unsigned int* sa = ss_mm_3bit(genome_file, genome_size+1);
        printf("complete suffix array generated...\n");
        write_file(safn, (char*)sa, ((size_t)(genome_size+1))*((size_t)sizeof(int)));
        printf("complete suffix array saved to disk...\n");

        // compute bwt
        printf("started to generate bwt...");
        char* bwt = compute_bwt_3bit(genome_file, sa, genome_size+1, gfsz);
        write_file(bwtfn, bwt, (size_t)gfsz);
        printf("bwt saved to disk...\n");

        // verify result
        for(unsigned int i = 0; i < genome_size+1; i++)
        {
            char bp = get_bp_3bit(bwt, i);
            if(bp == '*')
            {
                printf("%d: Error!!!\n", i);
                return 0;
            }
        }
        printf("result verified...\n");

        // compute partial suffix array
        unsigned int *sa_pt = compute_partial_sa(sa,genome_size,sample_size);
        printf("partial suffix array generated...\n");
        write_file(psafn, (char*)sa_pt, (genome_size/sample_size)*sizeof(int));
        printf("partial suffix array saved to disk...\n");
    }
    else
    {
        printf("Usage 1: [PROGRAM NAME]\n");
        printf("Usage 2: [PROGRAM NAME] [FILE NAME] [USE LARGE]\n");
        return 0;
    }
}

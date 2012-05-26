#include "misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc > 3)
    {
        printf("Usage: [PROGRAM NAME] [FILE NAME] [USE LARGE]\n");
        return 0;
    }
    else if(argc == 3)
    {
        // initialize file name
        unsigned int fnlen = strlen(argv[1]);
        char* fn2bit = (char*) malloc(fnlen+6);
        strcpy(fn2bit, argv[1]);
        strcpy(fn2bit+fnlen, "_2bit");
        fn2bit[fnlen+5] = '\0';
        char* fn3bit = (char*) malloc(fnlen+6);
        strcpy(fn3bit, argv[1]);
        strcpy(fn3bit+fnlen, "_3bit");
        fn3bit[fnlen+5] = '\0';

        // create 2 bit and 3 bit genome
        unsigned int len = 30000;
        if(argv[2][0] == 't' || argv[2][0] == 'T')
            len = GENOME_SIZE;
        unsigned int fsz2bit = (2*len-1)/BYTE_SIZE + 1;
        unsigned int fsz3bit = (3*len-1)/BYTE_SIZE + 2;
        char* genome_2bit = (char*) malloc(fsz2bit*sizeof(char));
        char* genome_3bit = (char*) malloc(fsz3bit*sizeof(char));
        memset(genome_2bit, 0, fsz2bit);
        memset(genome_3bit, 0, fsz3bit);
        printf("generating file...\n");
        for(unsigned int i = 0; i < len; i++)
        {
            int rnd = randnum(0, 3);
            char bp = '*';
            switch(rnd)
            {
                case 0: bp = 'a'; break;
                case 1: bp = 'c'; break;
                case 2: bp = 'g'; break;
                case 3: bp = 't'; break;
            }
            write_bp_2bit(genome_2bit, i, bp);
            write_bp_3bit(genome_3bit, i, bp);
        }
        write_bp_3bit(genome_3bit, len, '$');

        // verify result
        printf("verifying result...\n");
        for(unsigned int i = 0; i < len - 1; i++)
        {
            char bp2 = get_bp_2bit(genome_2bit, i);
            char bp3 = get_bp_3bit(genome_3bit, i);
            if(bp2 != bp3)
            {
                printf("%d: %c != %c\n", i, bp2, bp3);
                return 0;
            }
        }

        // save to disk
        printf("saving to disk...\n");
        write_file(fn2bit, genome_2bit, fsz2bit);
        write_file(fn3bit, genome_3bit, fsz3bit);

        // free memory
        free(genome_2bit);
        free(genome_3bit);
        free(fn2bit);
        free(fn3bit);
    }
    else if(argc == 2)
    {
        char* genome3bit;
        read_file(argv[1], &genome3bit);
        unsigned int len = GENOME_SIZE;
        for(unsigned int i = 0; i < len; i++)
        {
            char bp = get_bp_3bit(genome3bit, i);
            if(bp == '*')
                printf("error!!!\n");
        }
    }
}

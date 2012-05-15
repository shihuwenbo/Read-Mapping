#include "misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> 

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        fprintf(stderr, "Usage: [PROGRAM] [IN FILE NAME] [OUT FILE NAME]\n");
        exit(1);
    }
    double file_size_db = ceil(3 * ((double) GENOME_SIZE) / 8 + 1);
    unsigned int file_size = (unsigned int) file_size_db;
    char* genome_3bit = (char*) malloc(file_size * sizeof(char));
    memset(genome_3bit, 0, file_size);
    printf("3 bit file initialized\n");
    char* genome_2bit;
    read_file(argv[1], &genome_2bit);
    printf("2 bit file loaded\n");
    for(unsigned int i = 0; i < GENOME_SIZE; i++)
    {
        char bp = get_bp_2bit(genome_2bit, i);
        write_bp_3bit(genome_3bit, i, bp);
    }
    printf("2 bit file converted\n");
    write_file(argv[2], genome_3bit, file_size);
}

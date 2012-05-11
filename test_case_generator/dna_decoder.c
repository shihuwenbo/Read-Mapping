/* Encoding Rule: A: 00 (0), T: 01 (1), C: 10 (2), G: 11 (3) */

#include "coder_common.h"

/* open the file named file_name and read it into memory genome */
void read_file(char* file_name, char** genome);

/* get the dna at a location */
char get_dna(char* genome, size_t pos);

int main(int argc, char* argv[])
{
    if(argc != 2)
        error(1, 0, "Usage: [PROGRAM NAME] [GENOME FILE NAME]");
    char* genome;
    read_file(argv[1], &genome);
    size_t i;
    for(i = 0; i < GENOME_SIZE; i++)
        printf("%c", get_dna(genome, i));
    printf("%c", '\n');
}

void read_file(char* file_name, char** genome)
{
    FILE* file = fopen(file_name, "r");
    fseek(file, SEEK_SET, SEEK_END);
    size_t len = ftell(file);
    rewind(file);
    (*genome) = (char*) malloc(len);
    fread(*genome, sizeof(char), len, file);
    fclose(file);
}

char get_dna(char* genome, size_t pos)
{
    size_t byte_pos = pos / (BYTE_SIZE / ENCODE_SIZE);
    size_t byte_off = pos % (BYTE_SIZE / ENCODE_SIZE);
    unsigned char dna = genome[byte_pos];
    dna = dna << (byte_off * ENCODE_SIZE);
    dna = dna >> (BYTE_SIZE - ENCODE_SIZE);
    switch(dna)
    {
        case 0:
            return 'A';
        case 1:
            return 'T';
        case 2:
            return 'C';
        case 3:
            return 'G';
        default:
            error(1, 0, "Invlaid DNA");
    }
}

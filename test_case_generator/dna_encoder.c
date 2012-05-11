/* Encoding Rule: A: 00 (0), T: 01 (1), C: 10 (2), G: 11 (3) */

#include "coder_common.h"

/* make seed for the random number generator */
void make_seed();

/* build the genome, genome_size number of dna in the genome */
char* generate_genome(size_t genome_size);

/* generate a random number from low to high inclusively */
int rand_num(int low, int high);

/* write a dna at a position in the genome */
void write_genome(char* genome, char val, size_t pos);

int main(int argc, char* argv[])
{
    if(argc != 2)
        error(1, 0, "Usage: [PROGRAM NAME] [GENOME FILE NAME]");
    char* genome_data = generate_genome(GENOME_SIZE);
    FILE* genome_file = fopen(argv[1], "wb");
    int num_bytes = (GENOME_SIZE - 1) / (BYTE_SIZE / ENCODE_SIZE) + 1;
    fwrite(genome_data, 1, num_bytes, genome_file);
    fclose(genome_file);
    return 0;
}

void make_seed()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_sec * tv.tv_usec);
}

char* generate_genome(size_t genome_size)
{
    size_t num_bytes = (genome_size - 1) / (BYTE_SIZE / ENCODE_SIZE) + 1;
    char* genome = (char*) malloc(num_bytes);
    memset(genome, 0, num_bytes);
    size_t i;
    size_t A = 0, T = 0, C = 0, G = 0;
    for(i = 0; i < genome_size; i++)
    {
        int num = rand_num(0, 3);
        switch(num)
        {
            case 0:
            {
                printf("%c", 'A');
                A++;
                write_genome(genome, 'A', i);
                break;
            }
            case 1:
            {
                printf("%c", 'T');
                T++;
                write_genome(genome, 'T', i);
                break;
            }
            case 2:
            {
                printf("%c", 'C');
                C++;
                write_genome(genome, 'C', i);
                break;
            }
            case 3:
            {
                printf("%c", 'G');
                G++;
                write_genome(genome, 'G', i);
                break;
            }
        }
    }
    printf("%c", '\n');
    fprintf(stderr, "A: %d T: %d C: %d G: %d\n", A, T, C, G);
    return genome;
}

int rand_num(int low, int high)
{
    make_seed();
    return rand() % (high - low + 1) + low;
}

void write_genome(char* genome, char val, size_t pos)
{
    size_t byte_pos = pos / (BYTE_SIZE / ENCODE_SIZE);
    size_t byte_off = pos % (BYTE_SIZE / ENCODE_SIZE) + 1;
    switch(val)
    {
        case 'A':
        {
            char mask = 0 << (BYTE_SIZE - byte_off * ENCODE_SIZE);
            genome[byte_pos] |= mask;
            break;
        }
        case 'T':
        {
            char mask = 1 << (BYTE_SIZE - byte_off * ENCODE_SIZE);
            genome[byte_pos] |= mask;
            break;
        }
        case 'C':
        {
            char mask = 2 << (BYTE_SIZE - byte_off * ENCODE_SIZE);
            genome[byte_pos] |= mask;
            break;
        }
        case 'G':
        {
            char mask = 3 << (BYTE_SIZE - byte_off * ENCODE_SIZE);
            genome[byte_pos] |= mask;
            break;
        }
        default:
            error(1, 0, "Unidentified DNA");
    }
}

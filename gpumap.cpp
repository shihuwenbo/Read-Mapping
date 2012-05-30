#include "gpusearch.h"
#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

size_t read_file(char* file_name, char** genome);
void start_timer(double* time);
void stop_timer(double* time);
char get_bp_2bit(char* genome, unsigned int pos);
void write_bp_2bit(char* genome, unsigned int pos, char val);

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
        // initialize timer variable
        double ldtime = 0.0, sctime = 0.0, mcptimehd = 0.0;
        double mcptimedh = 0.0;

        // initlize genome and short read info
        unsigned int read_size = READ_SIZE;
        unsigned int genome_size = 3000001, sample_size = 10, read_num = 100000;
        if(argv[6][0] == 't')
        {
            genome_size = GENOME_SIZE;
            sample_size = SAMPLE_SIZE;
            read_num = READ_NUM;
        }

        // load in data
        char *bwt, *sr, *smlc, *occc, *psac; //*tsr;
        start_timer(&ldtime);
        size_t bwt_sz = read_file(argv[1], &bwt);
        size_t sr_sz = read_file(argv[2], &sr);
        size_t sml_sz = read_file(argv[3], &smlc);
        size_t occ_sz = read_file(argv[4], &occc);
        size_t psa_sz = read_file(argv[5], &psac);
        stop_timer(&ldtime);
        unsigned int* sml = (unsigned int*) smlc;
        unsigned int* occ = (unsigned int*) occc;
        unsigned int* psa = (unsigned int*) psac;
        // tsr = (char*) malloc(sr_sz*sizeof(char));

        // transpose short read
        /*
        for(unsigned int i = 0; i < READ_SIZE; i++)
        {
            for(unsigned int j = 0; j < read_num; j++)
            {
                char bp = get_bp_2bit(sr, j*READ_SIZE+i);
                write_bp_2bit(tsr, i*read_num+j, bp);
            }
        }
        */

        // assign space for answer
        unsigned int* all_ans = (unsigned int*)
                        malloc(read_num*sizeof(unsigned int));
        memset(all_ans, -1, read_num*sizeof(unsigned int));

        // initialize gpu memory
        char *gbwt, *gsr;
        unsigned int *gsml, *gocc, *gpsa, *gall_ans;
        gmalloc(&gbwt, bwt_sz);
        gmalloc(&gsr, sr_sz);
        gmalloc((char**)&gsml, sml_sz);
        gmalloc((char**)&gocc, occ_sz);
        gmalloc((char**)&gpsa, psa_sz);
        gmalloc((char**)&gall_ans, read_num*sizeof(unsigned int));

        // copy memory from host to device
        start_timer(&mcptimehd);
        gmemcpy_htod((char*)gbwt, (char*)bwt, bwt_sz*sizeof(char));
        gmemcpy_htod((char*)gsr,  (char*)sr,  sr_sz*sizeof(char));
        gmemcpy_htod((char*)gsml, (char*)sml, sml_sz*sizeof(char));
        gmemcpy_htod((char*)gocc, (char*)occ, occ_sz*sizeof(char));
        gmemcpy_htod((char*)gpsa, (char*)psa, psa_sz*sizeof(char));
        gmemcpy_htod((char*)gall_ans, (char*)all_ans, read_num*sizeof(unsigned int));
        stop_timer(&mcptimehd);

        // run the search using gpu
        int kerr = KERR;
        set_cache_size(2);
        set_stack_size(2400);
        start_timer(&sctime);
        gpu_search(gbwt,gsr,gpsa,read_num,read_size,gsml,gocc,gall_ans,kerr);
        stop_timer(&sctime);

        // copy answer back to host
        start_timer(&mcptimedh);
        gmemcpy_dtoh((char*)all_ans, (char*)gall_ans, read_num*sizeof(unsigned int));
        stop_timer(&mcptimedh);
        
        for(unsigned int i = 0; i < 100; i++)
        {
            printf("read #%u: ", i);
            if((int)all_ans[i] != -1)
                printf("%u", all_ans[i]);
            printf("\n");
        }

        fprintf(stderr, "Load time: %f sec\n", ldtime);
        fprintf(stderr, "Memcpy time (H to D): %f sec\n", mcptimehd);
        fprintf(stderr, "Search time: %f sec\n", sctime);
        fprintf(stderr, "Memcpy time (D to H): %f sec\n", mcptimedh);

        free(bwt);
        gfree((char*)gbwt);
        free(smlc);
        gfree((char*)gsml);
        free(occc);
        gfree((char*)gocc);
        free(sr);
        gfree((char*)gsr);
        free(psac);
        gfree((char*)gpsa);
        free(all_ans);
        gfree((char*)gall_ans);
    }
}

// read in file
size_t read_file(char* file_name, char** genome)
{
    FILE* file = fopen(file_name, "r");
    fseek(file, SEEK_SET, SEEK_END);
    size_t len = ftell(file);
    rewind(file);
    (*genome) = (char*) malloc(len);
    if(fread(*genome, sizeof(char), len, file) != len)
    {   
       fprintf(stderr, "failed to load file\n");
       exit(1);
    }   
    fclose(file);
    return (size_t) len;
}

// start timer
void start_timer(double* time)
{
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    unsigned int t = 1000000 * tv.tv_sec + tv.tv_usec;
    *time = t / 1000000.0;
}

// stop timer
void stop_timer(double* time)
{
    struct timeval tv; 
    gettimeofday(&tv, NULL);
    unsigned int t = 1000000 * tv.tv_sec + tv.tv_usec;
    double stop_time = (double) t;
    *time = (stop_time / 1000000.0) - *time;
}

// get a base pair at a position
char get_bp_2bit(char* genome, unsigned int pos)
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

void write_bp_2bit(char* genome, unsigned int pos, char val)
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

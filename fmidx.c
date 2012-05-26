#include "misc.h"
#include <stdio.h>
#include <math.h>

int main(int argc, char* argv[])
{
    if(argc != 4)
    {
        printf("Usage: [fmidx] [BWT FILE] [NUM] [SAMPLE SIZE]\n");
        return 0;
    }
    else
    {
        char* genome;
        read_file(argv[1], &genome);
        char *ptr;
        size_t num = (size_t) strtol(argv[2], &ptr, 10);
        size_t spsize = (size_t) strtol(argv[3], &ptr, 10);
        unsigned int* sml = (unsigned int*)
                            malloc((ALPHA_SIZE+1)*sizeof(unsigned int));
        unsigned int* occ = (unsigned int*)
                            malloc((4*num/spsize+4)*sizeof(unsigned int));
        unsigned int numd = 0, numa = 0, numc = 0, numg = 0, numt = 0;
        unsigned int smld = 0, smla = 0, smlc = 0, smlg = 0, smlt = 0;
        for(unsigned int i = 0, j = 0; i < num; i++)
        {
            char bp = get_bp_3bit(genome, i);
            if(bp < '$') smld++;
            if(bp < 'a') smla++;
            if(bp < 'c') smlc++;
            if(bp < 'g') smlg++;
            if(bp < 't') smlt++;

            if(bp == '$') numd++;
            if(bp == 'a') numa++;
            if(bp == 'c') numc++;
            if(bp == 'g') numg++;
            if(bp == 't') numt++;

            if(i % spsize == 0)
            {
                occ[j+0] = numa;
                occ[j+1] = numc;
                occ[j+2] = numg;
                occ[j+3] = numt;
                j += 4;
            }
        }

        if(smld > 0)
        {
            printf("Error: smld > 0\n");
            return 0;
        }

        if(numd > 1)
        {
            printf("Error: numd > 1\n");
            return 0;
        }

        sml[0] = smld;
        sml[1] = smla;
        sml[2] = smlc;
        sml[3] = smlg;
        sml[4] = smlt;

        /*
        printf("smld: %u, smla: %u, smlc: %u, smlg: %u, smlt: %u\n",
            smld, smla, smlc, smlg, smlt);
        */

        /*
        for(unsigned int i = 0; i < num/spsize; i++)
            printf("A: %u, C: %u, G: %u, T: %u\n", occ[i*4], occ[i*4+1], occ[i*4+2],occ[i*4+3]);
         */

        char smlfn[100];
        sprintf(smlfn, "%s_sml", argv[1]);
        write_file(smlfn, (char*)sml, (ALPHA_SIZE+1)*sizeof(unsigned int));
        
        char occfn[100];
        sprintf(occfn, "%s_occ", argv[1]);
        write_file(occfn, (char*)occ, ((4*num/spsize+4)*sizeof(unsigned int)));

        free(sml);
        free(occ);
        free(genome);
    }
}

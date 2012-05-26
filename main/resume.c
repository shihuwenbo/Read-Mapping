#include "suffix_sort.h"
#include "misc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int main()
{
    int* large = (int*) malloc(GENOME_SIZE*sizeof(int));
    write_file("../large", (char*)large, ((size_t)GENOME_SIZE)*((size_t)sizeof(int)));
    exit(1);
    time_t rawtime;
    struct tm *timeinfo;
    char buf[200];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P program started.",timeinfo);
    printf("%s\n",buf);

    char *posc, *prmc, *cntc, *bh, *b2h;
    read_file("pos512", &posc);
    read_file("prm512", &prmc);
    read_file("cnt512", &cntc);
    read_file("bh512", &bh);
    read_file("b2h512", &b2h);

    int* pos = (int*) posc;
    int* prm = (int*) prmc;
    int* count = (int*) cntc;

    unsigned int len = GENOME_SIZE + 1;
    unsigned int bit_vect_sz = (GENOME_SIZE + 1 - 1) / 8 + 2;

    /* inductive stages: sort pos incrementally in n*log(n) time */
    unsigned int j = 0;
    for(unsigned int H = 1024; H < len; H <<= 1)
    {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buf,200,"At %I:%M%P ",timeinfo);
        printf("%sH=%d started.\n",buf,H);

        // printf("=====================H: %d ====================\n", H);

    /* reset prm such that prm[i] points to the left most bucket */

        // find how many bins
        unsigned int num_bin = 0;
        for(unsigned int i = 0; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
                num_bin++;
        }

        // done if all the suffixes live in separate bins
        if(num_bin == len)
            break;

        // find the left most position of each bin
        for(unsigned int i = 0, j = 0; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
            {
                count[j] = i;
                j++;
            }
        }

        // set prm[i] to point to the left most in each bin
        for(unsigned int i = 0, j = -1; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
                j++;
            prm[pos[i]] = count[j];
        }

        // initialize count
        memset(count, 0, len*sizeof(unsigned int));

        // initialize b2h
        for(unsigned int i = 0; i < bit_vect_sz; i++)
            b2h[i] = bh[i];

        /*
        // print out information for debug
        printf("********************BEFORE********************\n");
        printf("i\tbh\tbh2\tprm\tpos\n");
        for(int i = 0; i < len; i++)
        {
            printf("%d\t%d\t%d\t%d\t%d = %s\n",
                    i,get_bit(bh,i),get_bit(b2h,i),prm[i],pos[i],str+pos[i]);
            if(i < len-1 && get_bit(bh,i+1) == 1)
                printf("----------------------------------------------\n");
        }
        */

        // scan bucket
        unsigned int l = 0, r = 0;
        while(l < len)
        {
            // find left and right boundary of a bucket
            for(r = l + 1; r < len; r++)
            {
                if(get_bit(bh, r) == 1)
                {
                    r = r -1;
                    break;
                }
            }
            if(r >= len)
                r = r - 1;

            // increment count, set prm, set b2h
            for(unsigned int i = l; i <= r; i++)
            {
                int Ti = (int) ((long long int)pos[i] - (long long int)H);
                if(Ti >= 0)
                {
                    count[prm[Ti]]++;
                    prm[Ti] += count[prm[Ti]] - 1;
                    set_bit(b2h, prm[Ti]);
                }
            }

            // update b2h
            for(unsigned int i = l; i <= r; i++)
            {
                int Ti = (int) ((long long int)pos[i] - (long long int)H);
                if(Ti >= 0)
                {
                    if(get_bit(b2h, prm[Ti]) == 1)
                    {
                        unsigned int j;
                        for(j = prm[Ti] + 1;
                            get_bit(bh,j)==0 && get_bit(b2h,j)==1; j++)
                            continue;
                        for(unsigned int k = prm[Ti] + 1; k < j; k++)
                            clear_bit(b2h, k);
                    }
                }
            }

            // update left side boundary
            l = r + 1;
        }

        // update pos and bh
        for(unsigned int i = 0; i < len; i++)
            pos[prm[i]] = i;

        for(unsigned int i = 0; i < bit_vect_sz; i++)
            bh[i] = b2h[i];

        /*
        // print out information for debug
        printf("********************AFTER*********************\n");
        printf("i\tbh\tbh2\tprm\tpos\n");
        for(int i = 0; i < len; i++)
        {
            printf("%d\t%d\t%d\t%d\t%d = %s\n",
                    i,get_bit(bh,i),get_bit(b2h,i),prm[i],pos[i],str+pos[i]);
            if(i < len-1 && get_bit(bh,i+1) == 1)
                printf("----------------------------------------------\n");
        }
        */

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buf,200,"At %I:%M%P ",timeinfo);
        printf("%sH=%d finished.\n",buf,H);
        j++;

        /* save state */
        if(j % 10 == 0)
        {
            char pos_file_name[20];
            sprintf(pos_file_name,"pos%d",H);
            write_file(pos_file_name, (char*)pos, (size_t)(len+1) * ((size_t)sizeof(unsigned int)));
            char prm_file_name[20];
            sprintf(prm_file_name,"prm%d",H);
            write_file(prm_file_name, (char*)prm, (size_t)(len+1) * ((size_t)sizeof(unsigned int)));
            char cnt_file_name[20];
            sprintf(cnt_file_name,"cnt%d",H);
            write_file(cnt_file_name, (char*)count,(size_t)(len+1) * ((size_t)sizeof(unsigned int)));
            char bh_file_name[20];
            sprintf(bh_file_name,"bh%d",H);
            write_file(bh_file_name, bh, bit_vect_sz * sizeof(char));
            char b2h_file_name[20];
            sprintf(b2h_file_name,"b2h%d",H);
            write_file(b2h_file_name, b2h, bit_vect_sz * sizeof(char));
        }
    }
    write_file("genome_file_3bit_sa", (char*)pos, ((size_t)(GENOME_SIZE+1))*((size_t)sizeof(int)));
}

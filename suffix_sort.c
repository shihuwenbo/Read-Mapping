#include "suffix_sort.h"
#include "misc.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

/*---------------------------------------------------------------------------*/
// struct for naive suffix sort algorithm
typedef struct str_int
{
    char* str;
    int idx;
}
str_int_t;

// comparator for qsort used in suffx sorting algorithm
int suffix_compare(const void* lhs, const void* rhs)
{
    str_int_t* lhs_sit = (str_int_t*) lhs;
    str_int_t* rhs_sit = (str_int_t*) rhs;
    return strcmp(lhs_sit->str, rhs_sit->str);
}

// naive suffix sorting algorithm
int* ss_naive_8bit(char* str, int len)
{
    // generate the suffixes
    str_int_t* suffixes = (str_int_t*) malloc(len*sizeof(str_int_t));
    for(int i = 0; i < len; i++)
    {
        suffixes[i].str = str++;
        suffixes[i].idx = i;
    }

    // sort the suffixes
    qsort(suffixes, len, sizeof(str_int_t), suffix_compare);

    // generate the suffix array
    int* sa = (int*) malloc(len*sizeof(int));
    for(int i = 0; i < len; i++)
        sa[i] = suffixes[i].idx;

    // return the result
    return sa;
}

/*---------------------------------------------------------------------------*/
// suffix sort by manber and myers
int* ss_mm_8bit(char* str, int len)
{

    /* allocating memory for arrays required for sorting */

    int* pos = (int*) malloc((len+1) * sizeof(int));
    int* prm = (int*) malloc((len+1) * sizeof(int));
    int* count = (int*) malloc((len+1) * sizeof(int));
    int bit_vect_sz = (len - 1) / 8 + 2;
    char* bh = (char*) malloc(bit_vect_sz * sizeof(char));
    char* b2h = (char*) malloc(bit_vect_sz * sizeof(char));

    /* initialize boolean bit vectors */
    memset(bh, 0, bit_vect_sz*sizeof(char));
    memset(bh, 0, bit_vect_sz*sizeof(char));

    /* first stage: radix sorting according to the first letter */

    // initialize the bin for counting sort
    int bin[ALPHA_SIZE + 1];
    memset(bin, 0, (ALPHA_SIZE+1)*sizeof(int));

    // count the number of each letter
    for(int i = 0; i < len; i++)
        bin[alpha_rank(str[i])]++;

    // compute offset of each letter
    int offset[ALPHA_SIZE + 1];
    offset[0] = 0;
    for(unsigned int i = 1; i < ALPHA_SIZE + 1; i++)
        offset[i] = offset[i-1] + bin[i-1];

    // use the offset to compute the first stage suffix array
    for(int i = 0; i < len; i++)
        pos[offset[alpha_rank(str[i])]++] = i;

    // compute the first stage prm
    for(int i = 0; i < len; i++)
        prm[pos[i]] = i;

    // compute the first stage bh
    set_bit(bh, 0);
    for(int i = 1; i < len; i++)
    {
        if(str[pos[i]] != str[pos[i-1]])
            set_bit(bh, i);
    }

    /* inductive stages: sort pos incrementally in n*log(n) time */

    for(int H = 1; H < len; H <<= 1)
    {

        // printf("=====================H: %d ====================\n", H);

    /* reset prm such that prm[i] points to the left most bucket */

        // find how many bins
        int num_bin = 0;
        for(int i = 0; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
                num_bin++;
        }

        // done if all the suffixes live in separate bins
        if(num_bin == len)
            break;

        // find the left most position of each bin
        for(int i = 0, j = 0; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
            {
                count[j] = i;
                j++;
            }
        }

        // set prm[i] to point to the left most in each bin
        for(int i = 0, j = -1; i < len; i++)
        {
            if(get_bit(bh, i) == 1)
                j++;
            prm[pos[i]] = count[j];
        }

        // initialize count
        memset(count, 0, len*sizeof(int));

        // initialize b2h
        for(int i = 0; i < bit_vect_sz; i++)
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
        int l = 0, r = 0;
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
            for(int i = l; i <= r; i++)
            {
                int Ti = pos[i] - H;
                if(Ti >= 0)
                {
                    count[prm[Ti]]++;
                    prm[Ti] += count[prm[Ti]] - 1;
                    set_bit(b2h, prm[Ti]);
                }
            }

            // update b2h
            for(int i = l; i <= r; i++)
            {
                int Ti = pos[i] - H;
                if(Ti >= 0)
                {
                    if(get_bit(b2h, prm[Ti]) == 1)
                    {
                        int j;
                        for(j = prm[Ti] + 1;
                            get_bit(bh,j)==0 && get_bit(b2h,j)==1; j++)
                            continue;
                        for(int k = prm[Ti] + 1; k < j; k++)
                            clear_bit(b2h, k);
                    }
                }
            }

            // update left side boundary
            l = r + 1;
        }

        // update pos and bh
        for(int i = 0; i < len; i++)
            pos[prm[i]] = i;

        for(int i = 0; i < bit_vect_sz; i++)
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
    }

    /* free dynamically allocated memory */

    free(prm);
    free(bh);
    free(b2h);
    free(count);

    return pos;
}

/*---------------------------------------------------------------------------*/
// suffix sort by manber and myers
unsigned int* ss_mm_3bit(char* str, unsigned int len)
{
    time_t rawtime;
    struct tm *timeinfo;
    char buf[200];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P program started.",timeinfo);
    printf("%s\n",buf);

    /* allocating memory for arrays required for sorting */

    unsigned int* pos = (unsigned int*) malloc((size_t)(len+1) * ((size_t)sizeof(unsigned int)));
    unsigned int* prm = (unsigned int*) malloc((size_t)(len+1) * ((size_t)sizeof(unsigned int)));
    unsigned int* count = (unsigned int*) malloc((size_t)(len+1) * ((size_t)sizeof(unsigned int)));
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P space allocated.",timeinfo);
    printf("%s\n",buf);

    unsigned int bit_vect_sz = (len - 1) / 8 + 2;
    char* bh = (char*) malloc(bit_vect_sz * sizeof(char));
    char* b2h = (char*) malloc(bit_vect_sz * sizeof(char));

    /* initialize boolean bit vectors */
    memset(bh, 0, bit_vect_sz*sizeof(char));
    memset(bh, 0, bit_vect_sz*sizeof(char));

    /* first stage: radix sorting according to the first letter */

    // initialize the bin for counting sort
    unsigned int bin[ALPHA_SIZE + 1];
    memset(bin, 0, (ALPHA_SIZE+1)*sizeof(unsigned int));

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P zeroed.",timeinfo);
    printf("%s\n",buf);

    // count the number of each letter
    for(unsigned int i = 0; i < len; i++)
    {
        char bp = get_bp_3bit(str, (unsigned long long int)i);
        if(bp == '*')
            exit(1);
        bin[alpha_rank(bp)]++;
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P counted.",timeinfo);
    printf("%s\n",buf);

    // compute offset of each letter
    unsigned int offset[ALPHA_SIZE + 1];
    offset[0] = 0;
    for(unsigned int i = 1; i < ALPHA_SIZE + 1; i++)
        offset[i] = offset[i-1] + bin[i-1];

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P offset computed.",timeinfo);
    printf("%s\n",buf);


    // use the offset to compute the first stage suffix array
    for(unsigned int i = 0; i < len; i++)
    {
        char bp = get_bp_3bit(str, (unsigned long long int)i);
        if(bp == '*')
            exit(1);
        pos[((unsigned long long int) offset[alpha_rank(bp)]++)] = i;
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P first sa done.",timeinfo);
    printf("%s\n",buf);

    // compute the first stage prm
    for(unsigned int i = 0; i < len; i++)
        prm[pos[i]] = i;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P first sa-1 done.",timeinfo);
    printf("%s\n",buf);

    // compute the first stage bh
    set_bit(bh, 0);
    for(unsigned int i = 1; i < len; i++)
    {
        char bpl = get_bp_3bit(str, pos[i]);
        if(bpl == '*')
        {
            printf("%i: invalid alpha...\n", i);
            exit(1);
        }
        char bpr = get_bp_3bit(str, pos[i-1]);
        if(bpr == '*')
        {
            printf("%i: invalid alpha...\n", i);
            exit(1);
        }
        if(bpl != bpr)
            set_bit(bh, i);
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf,200,"At %I:%M%P initialized.",timeinfo);
    printf("%s\n",buf);

    /* inductive stages: sort pos incrementally in n*log(n) time */
    unsigned int j = 0;
    for(unsigned int H = 1; H < len; H <<= 1)
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
        memset(count, 0, ((size_t)len)*((size_t)sizeof(unsigned int)));

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
                long long int Ti = (long long int) ((long long int)pos[i] - (long long int)H);
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
                long long int Ti = (long long int) ((long long int)pos[i] - (long long int)H);
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
        /*
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
        */
    }

    /* free dynamically allocated memory */

    free(prm);
    free(bh);
    free(b2h);
    free(count);

    return pos;
}


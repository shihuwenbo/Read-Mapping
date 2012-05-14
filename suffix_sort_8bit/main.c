#include "suffix_sort.h"
#include "misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if(argc >= 2)
    {
        // for timing in microsecond
        double time;

        // suffix array by naive sort
        start_timer(&time);
        int* sa0 = ss_naive(argv[1], strlen(argv[1]));
        stop_timer(&time);
        printf("ss_naive: %f\tmsec\n", time);

        // suffix array by manber and myers algorithm
        start_timer(&time);
        int* sa1 = ss_mm(argv[1], strlen(argv[1]));
        stop_timer(&time);
        printf("ss_mm:    %f\tmsec\n", time);

        // verify result
        for(int i = 0; i < strlen(argv[1]); i++)
        {
            if(sa0[i] != sa1[i])
            {
                printf("Test failed!\n");
                return 0;
            }
        }

        /*
        // print out suffix array
        printf("suffix array: ");
        for(int i = 0; i < strlen(argv[1]); i++)
            printf("%d ", sa1[i]);
        printf("\n");
        */

        return 0;
    }

    // compare performance
    for(int i = 0; i < 300000; i++)
    {
        // generate test string
        char* test = (char*) malloc((i+2)*sizeof(char));
        test[i+1] = '\0';
        test[i] = '$';
        for(int j = 0; j < i; j++)
        {
            int rnd = randnum(0, 3);
            switch(rnd)
            {
                case 0: test[j] = 'a'; break;
                case 1: test[j] = 'c'; break;
                case 2: test[j] = 'g'; break;
                case 3: test[j] = 't'; break;
            }
        }

        // for timing
        double time_naive = 0.0, time_mm = 0.0;

        // suffix array by naive sort
        start_timer(&time_naive);
        int* sa0 = ss_naive(test, i + 1);
        stop_timer(&time_naive);

        // suffix array by manber and myers algorithm
        start_timer(&time_mm);
        int* sa1 = ss_mm(test, i + 1);
        stop_timer(&time_mm);

        // verify result
        for(int j = 0; j < i + 1; j++)
        {
            if(sa0[j] != sa1[j])
            {
                printf("Test failed!\n");
                return 0;
            }
        }

        // print out result
        // printf("%d, %f, %f\n", i, time_naive, time_mm);

        // free up memory
        free(test);
        free(sa0);
        free(sa1);
    }

    return 0;
}

#include "suffix_sort.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // for timing in microsecond
    int time;

    // suffix array by naive sort
    start_timer(&time);
    int* sa0 = ss_naive(argv[1], strlen(argv[1]));
    stop_timer(&time);
    printf("ss_naive: %d\tmsec\n", time);

    // suffix array by manber and myers algorithm
    start_timer(&time);
    int* sa1 = ss_mm(argv[1], strlen(argv[1]));
    stop_timer(&time);
    printf("ss_mm:    %d\tmsec\n", time);

    // verify result
    for(int i = 0; i < strlen(argv[1]); i++)
    {
        if(sa0[i] != sa1[i])
        {
            printf("Test failed!\n");
            return 0;
        }
    }

    // print out suffix array
    printf("suffix array: ");
    for(int i = 0; i < strlen(argv[1]); i++)
        printf("%d ", sa1[i]);
    printf("\n");

    return 0;
}

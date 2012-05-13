#include "suffix_sort.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    // test case
    char test[] = "actaagaacaccgtggaacg$";

    // suffix array by naive sort
    int* sa0 = suffix_sorting_0(test, strlen(test));
    printf("Suffix Array 0: ");
    for(int i = 0; i < strlen(test); i++)
        printf("%d ", sa0[i]);
    printf("\n");

    // suffix array by manber and myers algorithm
    int* sa1 = suffix_sorting_1(test, strlen(test));
    printf("Suffix Array 1: ");
    for(int i = 0; i < strlen(test); i++)
        printf("%d ", sa1[i]);
    printf("\n");

    // verify result
    for(int i = 0; i < strlen(test); i++)
    {
        if(sa0[i] != sa1[i])
        {
            printf("Failed :(\n");
            return 0;
        }
    }

    // Passed test
    printf("Passed!!!\n");

    return 0;
}

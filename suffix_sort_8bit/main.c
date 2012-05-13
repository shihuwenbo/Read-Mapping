#include "suffix_sort.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    // suffix array by naive sort
    int* sa0 = ss_naive(argv[1], strlen(argv[1]));

    // suffix array by manber and myers algorithm
    int* sa1 = ss_mm_refined(argv[1], strlen(argv[1]));

    // verify result
    for(int i = 0; i < strlen(argv[1]); i++)
    {
        if(sa0[i] != sa1[i])
        {
            printf("Failed :(\n");
            return 0;
        }
    }

    // Passed test
    printf("Passed :)\n");

    return 0;
}

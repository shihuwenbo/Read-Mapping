#include "suffix_sort.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    char test[] = "acaacg$";
    int* sa0 = suffix_sorting_0(test, strlen(test));
    printf("Suffix Array 0: ");
    for(int i = 0; i < strlen(test); i++)
        printf("%d ", sa0[i]);
    printf("\n");
    int* sa1 = suffix_sorting_1(test, strlen(test));
    printf("Suffix Array 1: ");
    for(int i = 0; i < strlen(test); i++)
        printf("%d ", sa1[i]);
    printf("\n");
    return 0;
}

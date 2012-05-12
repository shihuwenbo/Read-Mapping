#include "suffix_sort.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    char test[] = "acaacg$";
    int* sa = suffix_sorting_2(test, strlen(test));
    for(int i = 0; i < strlen(test); i++)
        printf("%d ", sa[i]);
    printf("\n");
    return 0;
}

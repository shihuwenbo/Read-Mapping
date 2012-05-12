#include "suffix_sort.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[])
{
    char test[] = "acbaacatat$";
    suffix_sorting_2(test, strlen(test));
    return 0;
}

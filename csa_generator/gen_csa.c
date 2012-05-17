#include "csa.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "misc.h"

int main()
{
    // generate a 10000 len string
    char* test = (char*) malloc(10002*sizeof(char));
    test[10001] = '\0';
    test[10000] = '$';
    for(int i = 0; i < 10000; i++)
    {
        int rnd = randnum(0,3);
        switch(rnd)
        {
            case 0: test[i] = 'a'; break;
            case 1: test[i] = 'c'; break;
            case 2: test[i] = 'g'; break;
            case 3: test[i] = 't'; break;
        }
    }
    gen_csa(test, strlen(test));
}

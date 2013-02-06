#include <stdlib.h>
#include <stdio.h>
#include "string.h"

int main(int argc, const char *argv[])
{
    char *c= "Un test";
    int len = strlen(c);
    printf("La longueur de \'Un test\' est de : %d \n", len);
    return 0;
}
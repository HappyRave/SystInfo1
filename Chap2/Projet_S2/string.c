#include <stdlib.h>
#include <stdio.h>

int strlen(char *s)
{
    int i=0;
    
    while (s[i] != '\0') {
        i++;
    }
    return i;
}
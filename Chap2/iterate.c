#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
    
    
    //Facon 1
    
    char *ptr = "Test";
    
    while (*ptr != '\0') {
        printf("%c \n", *ptr);
        printf("%p \n", &(*ptr));
        ptr++;
    }
    
    //Facon 2
    
    char *ptr2 = "Test";
    int i=0;
    
    while (ptr2[i] != '\0') {
        printf("%c \n", ptr2[i]);
        printf("%p \n", &ptr2[i]);
        i++;
    }
    exit(0);
}
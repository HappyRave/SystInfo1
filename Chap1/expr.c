#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    int a=atoi(argv[1]);
    int b=atoi(argv[3]);
    char e=*argv[2];
    
    if (e=='+') {
        printf("%d\n",a+b);
    }
    else if (e=='*') {
        printf("%d\n",a*b);
    }
    else if (e=='-') {
        printf("%d\n",a-b);
    }
    else if (e=='/') {
        printf("%d\n",a/b);
    }
    else if (e=='%') {
        printf("%d\n",a%b);
    }
    else {
        printf("Syntax Error\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
    
}
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, const char *argv[])
{
    if (argc==1) {
        printf("Missing Argument!");
        exit(EXIT_FAILURE);
    } else {
        int i=atoi(argv[1]);
        
        int bin=0;
        int count=0;
        
        while (bin==0) {
            if ((i & 1)==1) {
                bin=pow(2,count);
            } else {
                count++;
                i=i >> 1;
            }
        }
        
        printf("%d\n",bin);
        exit(EXIT_SUCCESS);
    }
}
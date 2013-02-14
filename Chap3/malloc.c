#include <stdlib.h>
#include <stdio.h>

int global;
int main(void)
{
        int local;
        int *ptr1 = (int *)malloc(sizeof(*ptr1));
        int *ptr2 = (int *)malloc(sizeof(*ptr2));

        printf("global %p loc %p p1 %p p2 %p\n", &global, &local, ptr1, ptr2);
}

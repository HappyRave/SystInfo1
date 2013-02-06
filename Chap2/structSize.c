#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
    typedef struct foo_t foo;
    struct foo_t {
        char a;
        int b;
    };
    
    typedef struct foo2_t foo2;
    struct foo2_t {
        char a;
        int b;
        int c;
    };

    
    char c='a';
    int i=3;
    foo f={c, i};
    
    int sizeC=sizeof c;
    int sizeI=sizeof i;
    int sizeF = sizeof f;
    printf("La taille en mémoire de la structure foo_t est de %d octets\n", sizeF);
    printf("Un char et un int occupent respectivement %d et %2d octets\n", sizeC, sizeI);
    
    foo2 f2={'a',3,4};
    int sizeF2=sizeof f2;
    printf("Si on rajoute un deuxieme int dans foo_t, la taille passe à %d octets\n", sizeF2);

    
}
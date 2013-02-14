#include <stdlib.h>
#include <stdio.h>

typedef struct test_t test_t;
struct test_t {
        char c;
        long l;
        short s;
} __attribute__((packed));

int main(int argc, const char *agrv[])
{
  test_t test ={'a',(long)1337,(short)12};
  printf("La taille de test_t est de %d octets.\n", (int)sizeof(test));
  printf("La taille d'un char est de %d octets et l'adresse de ce char dans la mémoir est %p.\n", (int)sizeof('a'),&(test.c));
  printf("La taille d'un long est de %d octets et l'adresse de ce long dans la mémoire est %p.\n", (int)sizeof((long)1337),&(test.l));
  printf("La taille d'un short est de %d octets et l'adresse de ce short dans la mémoire est %p.\n", (int)sizeof((short)12), &(test.s));
  printf("Les variables de la structure test ne se suivent donc pas vraiment dans la mémoire\nChaque variable se voit attribuer un bloc de 8 octets.\npacked permet de coller les variables, mais ça devient moins facile à manipuler pour l'ordinateur");
}

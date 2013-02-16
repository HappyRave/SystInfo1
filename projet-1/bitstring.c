#include "bitstring.h"
#include <stdio.h>

bitstring_t *bitstring_alloc(size_t n)
{
	if (n <= 0) { // la longueur d'un bistring doit être naturelment strictement supérieur à 0
		return NULL;
	} else if ( n%8!=0 ) { // n doit être un multiple de 8
		return NULL;
	}
	bit_t *bits = (bit_t*)calloc(n, sizeof(bit_t)); // alloue la mémoire pour la chaine de bits. calloc initialise cette mémoire à 0
	bitstring_t* bitstring = (bitstring_t*)malloc(sizeof(bitstring_t));
	
	// renplir notre structure
	bitstring->len = n;
	bitstring->bits = bits;
	
	return bitstring;
}

bitstring_t *bitstring_alloc_from_int(unsigned int x)
{
	int size=(int)(sizeof(int)*8);
	bit_t *bits = (bit_t*)malloc(sizeof(bit_t)*size);
	
	int i;
	for (i=0; i<size; i++) {
		bit_t b = (x >> i) & 1;
		*(bits+i)=b;
	}
	
	bitstring_t* bitstring = (bitstring_t*)malloc(sizeof(bitstring_t));
	
	// renplir notre structure
	bitstring->len = size;
	bitstring->bits = bits;

	return bitstring;
}

void bitstring_set(bitstring_t *b, unsigned int n, bit_t bit)
{
	if (n >= (b->len)) {
		printf("Erreur dans bitstring_set(bitstring_t *b, unsigned int n, bit_t bit)\n'n' est plus grand ou égal à 0 et strictement plus petit que bitstring_len(b)\n");
		exit(-1);
	}
	bit_t *bits = b->bits;
	*(bits+n) = bit;
}

bit_t bitstring_get(bitstring_t *b, int n)
{
	if (n < 0 || n >= (b->len)) {
		printf("Erreur dans bitstring_get(bitstring_t *b, int n, bit_t bit)\n'n' est plus grand ou égal à 0 et strictement plus petit que bitstring_len(b)\n");
		exit(-1);
	}

	bit_t *bits = b->bits;
	return *(bits+n);
}

size_t bitstring_len(bitstring_t *b)
{
	return((size_t)b->len);
}

void bitstring_free(bitstring_t *b)
{
	if (b!=NULL) {
	free(b->bits);
	free(b);
	}
}
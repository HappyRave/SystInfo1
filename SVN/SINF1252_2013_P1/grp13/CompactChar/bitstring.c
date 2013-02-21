#include "bitstring.h"
#include <stdio.h>

struct bitstring
{
	int len; // longueur du bitstring
	char *bytes; // tableau de bytes (8 bits)
};

bitstring_t *bitstring_alloc(size_t n)
{
	if (n <= 0)
	{ // la longueur d'un bistring doit être naturelment strictement supérieur à 0
		return NULL;
	}
	else if ( n%8!=0 )
	{ // n doit être un multiple de 8
		return NULL;
	}
	char *bytes = (char*)calloc(n/8, sizeof(char)); // alloue la mémoire pour la chaine de bits. calloc initialise cette mémoire à 0
	
	if (bytes == NULL)
	{
		return NULL;
	}
	
	bitstring_t* bitstring = (bitstring_t*)malloc(sizeof(bitstring_t));
	
	if (bitstring == NULL)
	{
		return NULL;
	}
	
	// renplir notre structure
	bitstring->len = n;
	bitstring->bytes = bytes;
	
	return bitstring;
}

bitstring_t *bitstring_alloc_from_int(unsigned int x)
{
	bitstring_t *b = bitstring_alloc(32); // les int ont une longeur de 4 bytes
	int i;
	for (i=0; i<32; i++) {
		bitstring_set(b,i,((x >> i) & 1));
	}
	return b;
}

void bitstring_set(bitstring_t *b, unsigned int n, bit_t bit)
{
	if (n >= (b->len)) {
		fprintf(stderr,"Erreur dans bitstring_set(bitstring_t *b, unsigned int n, bit_t bit)\n'n' est plus grand ou égal à 0 et strictement plus petit que bitstring_len(b)\n");
		exit(-1);
	}
	
	if (bitstring_get(b,n) != bit)
		*(b->bytes + n/8) ^= (1 << n%8);
}

bit_t bitstring_get(bitstring_t *b, int n)
{
	if ((n < 0) || (n >= (b->len)))
	{
		fprintf(stderr,"Erreur dans bitstring_get(bitstring_t *b, int n, bit_t bit)\n'n' est plus grand ou égal à 0 et strictement plus petit que bitstring_len(b)\n");
		exit(-1);
	}
	return((*(b->bytes+n/8) >> n%8) & 1);
}

size_t bitstring_len(bitstring_t *b)
{
	return((size_t)b->len);
}

void bitstring_free(bitstring_t *b)
{
	if (b!=NULL)
	{ // ne libère la mémoire que si b n'est pas déjà null
		if (b->bytes != NULL)
		{// on verifie la même chose pour bytes, même si, théoriquement, ceci ne va jamais trigger
			free(b->bytes);
		}
		free(b);
	}
}
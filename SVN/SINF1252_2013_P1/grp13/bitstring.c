#include "bitstring.h"
#include <stdio.h>

struct bitstring
{
	int len; // longueur du bitstring
	char *bits; // tableau de bits (0 ou 1)
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
	char *bits = (char*)calloc(n, sizeof(bit_t)); // alloue la mémoire pour la chaine de bits. calloc initialise cette mémoire à 0
	
	if (bits == NULL)
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
	bitstring->bits = bits;
	
	return bitstring;
}

bitstring_t *bitstring_alloc_from_int(unsigned int x)
{
	char *bits = (char*)malloc(sizeof(bit_t)*32);

	if (bits == NULL)
	{
	  return NULL;
	}
	
	int i;
	for (i=0; i<32; i++) {
	        char b = (x >> i) & 1;
		*(bits+i)=b;
	}
	
	bitstring_t* bitstring = (bitstring_t*)malloc(sizeof(bitstring_t));

	if (bitstring == NULL)
	{
	  return NULL;
	}
	
	// renplir notre structure
	bitstring->len = 32;
	bitstring->bits = bits;

	return bitstring;
}

void bitstring_set(bitstring_t *b, unsigned int n, bit_t bit)
{
        if (n >= (b->len)) 
	{
	  fprintf(stderr,"Erreur dans bitstring_set(bitstring_t *b, unsigned int n, bit_t bit)\n'n' est plus grand ou égal à 0 et strictement plus petit que bitstring_len(b)\n");
	  exit(-1);
	}
	char *bits = b->bits;
	*(bits+n) = (char)bit;
}

bit_t bitstring_get(bitstring_t *b, int n)
{
        if ((n < 0) || (n >= (b->len))) 
        {
	  fprintf(stderr,"Erreur dans bitstring_get(bitstring_t *b, int n, bit_t bit)\n'n' est plus grand ou égal à 0 et strictement plus petit que bitstring_len(b)\n");
	  exit(-1);
	}

	char *bits = b->bits;
	return *(bits+n);
}

size_t bitstring_len(bitstring_t *b)
{
	return((size_t)b->len);
}

void bitstring_rotate(bitstring_t *b, int n)
{
        n = n%(b->len);//appliquer une rotation où n vaut le nombre de bit n'a aucun effet
	int nbshift = 0;//variable qui donne le nombre d'élément qu'on a shifté
	char toshift = *(b->bits);//bit_t qu'on va déplacer au prochain passage dans la boucle
	int shift = 0;//indice dans le bitstring du bit qu'on va shifter
	int depl;
       
	char transfer;//variable permettant le transfer des bit dans la boucle
	
        while (nbshift<(b->len))//il faut shifter tous les éléments
        {
	  depl = shift - n;//depl est l'indice où l'on va changer le bit dans le bitstring. Il faut traiter le cas où il est négatif ou plus grand que l'indice len-1 (le dernier indice dans le bitstring correspond à len-1 car le 1e élément est d'indice 0)
	  if (depl<0)
	  {
	    depl = b->len + depl;
	  }
	  else if (depl>((b->len)-1))
	  {
	    depl = depl - (b->len);
	  }
	  
	  transfer = toshift;
	  
	  toshift = *((b->bits)+depl);//élément qu'on garde pour le déplacement dans le prochain passage dans la boucle

	  *((b->bits)+depl) = transfer;
	  
	  shift = depl;
	  
	  nbshift = nbshift + 1;
        }
}

int bitstring_print(bitstring_t *b, char *buf, size_t len)
{
	size_t bitLen = (size_t)b->len;
	size_t hexLen = bitLen/4;
	if (hexLen>len) {
		return(-1);
	}
	
	char *bits = b->bits;
	
	int i;
	for (i=0; i<(int)hexLen; i++) {
		char *tmp = malloc(sizeof(char)*(4+1));
		*(tmp+4)='\0';
		int j;
		for (j=0; j<4; j++) {
			*(tmp+j) = (char)*(bits+(i*4)+j);
		}
		
	}
	return 0;
}

void bitstring_free(bitstring_t *b)
{
	if (b!=NULL) 
	{ // ne libère la mémoire que si b n'est pas déjà null
	  if (b->bits != NULL)
	  {
	    free(b->bits);
	  }
	  free(b);
	}
}

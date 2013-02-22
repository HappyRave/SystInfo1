//
//  Created by Cyril De Bodt and Maxime De Mol on 22/02/13.
//  Copyright (c) 2013 grp13. All rights reserved.
//

#include "bitstring.h"
#include <stdio.h>

struct bitstring
{
	int len; // longueur du bitstring
	char *bytes; // tableau de bytes (8 bits)
};

bitstring_t *bitstring_alloc(size_t n)
{
	if (n <= 0) return NULL;
	// la longueur d'un bistring doit être naturelment strictement supérieur à 0. Sinon pas de bitstring :-(
	
	if ( n%8!=0 ) return NULL;
	// n doit être un multiple de 8. L'ordinateur manipule les bits par octets, et un char contient exactement 8 bits

	char *bytes = (char*)calloc(n/8, sizeof(char));
	// alloue la mémoire pour la chaine de bits. calloc initialise cette mémoire à 0
	
	if (bytes == NULL) return NULL;
	// toujours vérifier si l'allocation s'est bien déroulé
	
	bitstring_t* bitstring = (bitstring_t*)malloc(sizeof(bitstring_t));
	
	if (bitstring == NULL) return NULL;
	
	// renplir notre structure
	bitstring->len = n;
	bitstring->bytes = bytes;
	
	return bitstring;
}

bitstring_t *bitstring_alloc_from_int(unsigned int x)
{
	bitstring_t *b = bitstring_alloc(32);
	// les int ont une longeur de 4 bytes
	
	if (b == NULL) return NULL;
	
	int i;
	for (i=0; i<32; i++)
		bitstring_set(b,i,((x >> i) & 1));
	
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
	// Un peu tricky comme opération. Comme nous allons utiliser set (et get) quasi tout le temps, on voulait rendre la fonction la plus efficace possible. 'n/8' renvoit permet de se trouver le char qui contient le n ième bit, et 'n%8' permet de trouver la place du n ième bit dans ce char. 
}

bit_t bitstring_get(bitstring_t *b, int n)
{
	if ((n < 0) || (n >= (b->len))) {
		fprintf(stderr,"Erreur dans bitstring_get(bitstring_t *b, int n, bit_t bit)\n'n' est plus grand ou égal à 0 et strictement plus petit que bitstring_len(b)\n");
		exit(-1);
	}
	
	return((*(b->bytes+n/8) >> n%8) & 1);
}

size_t bitstring_len(bitstring_t *b)
{
	return((size_t)b->len);
}

void bitstring_rotate(bitstring_t *b, int n)
{
	n = n%(b->len);//appliquer une rotation où n vaut le nombre de bit n'a aucun effet
	
	bitstring_t *copi = bitstring_alloc(b->len);//on crée une nouvelle structure qui va contenir la rotation de la première
	
	//Remarque : si n est positif, on décale vers la gauche, donc on augmente l'indice du bit dans la structure.
	//si n est négatif, on décale vers la droite, donc on diminue l'indice du bit dans la structure
	int i;
	for(i=0;i<32;i++)
	{
		if (i+n<0)
		{
			bitstring_set(copi,32+i+n,bitstring_get(b,i));
		}
		else if(i+n>31)
		{
			bitstring_set(copi,i+n-32,bitstring_get(b,i));
		}
		else
		{
			bitstring_set(copi,i+n,bitstring_get(b,i));
		}
	}
	
	//on met à jour les bits du bitstring passe en arguments.
	
	b->bits = copi->bits;
	
	//on libère la mémoire déagée pour la première structure :
	
	bitstring_free(copi);
	
}

int bitstring_concat(bitstring_t *b1, bitstring_t *b2)
{
	//
}


int bitstring_print(bitstring_t *b, char *buf, size_t len)
{
	int hexLen = b->len/4; // longeur max du bitstring en hexa
	char *tmp = (char*)malloc(sizeof(char)*hexLen+1); // alloc d'un char* temporaire
	if (tmp == NULL) fprintf(stderr, "Erreur à la création d'un char* dans bitstring_print\n");
	
	*(tmp+hexLen) = '\0'; // symbole de terminaison
	
	int i;
	for (i=0; i<(b->len)/8; i++) {
		unsigned char c = *(b->bytes + i);
		// nous allons convertir notre char c (8 bits) en 2 nombres hexadecimaux
		char hex1 = c%16;
		if (hex1 <= 9) hex1 +='0';
		else hex1 = hex1 - 10 + 'A';
		
		char hex2 = c/16;
		if (hex2 <= 9) hex2 +='0';
		else hex2 = hex2 - 10 + 'A';
		
		*(tmp+hexLen - 1 - 2*i) = hex1;
		*(tmp+hexLen - 2 - 2*i) = hex2;
	}
	
	int firstDigit=0;
	while (*(tmp+firstDigit) == '0') firstDigit++; // nous determinons le nombre de 0 avant le debut de la representation hexa
	
	if (len-1 < hexLen - firstDigit) { // si bu n'est pas assez grand pour contenir notre nombre
		free(tmp); // pas oublierde free tmp
		return(-1);
	}
	
	if (firstDigit == hexLen ) // cas particulier où nous avons que des 0
	{
		*buf = '0';
		*(buf + 1) = '\0';
		free(tmp);
		return 2;
	}
	
	int k;
	for (k = 0; k < hexLen - firstDigit; k++) *(buf + k) = *(tmp + k + firstDigit);
	// placer dans le buf la représentation hexa sans les 0 devant
	*(buf + k) = '\0';
	
	free(tmp);
	return k;
}

void bitstring_free(bitstring_t *b)
{
	if (b!=NULL) { // ne libère la mémoire que si b n'est pas déjà null
		if (b->bytes != NULL) {// on verifie la même chose pour bytes, même si, théoriquement, ceci ne va jamais trigger
			free(b->bytes);
		}
		free(b);
	}
}
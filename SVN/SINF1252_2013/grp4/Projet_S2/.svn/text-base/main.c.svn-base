#include <stdlib.h>
#include <stdio.h>
#include "string.h"

int main(int argc, const char *argv[])
{
	printf("Test de \'strlen\'\n");
    char *c= "Un test";
    int len = strlen(c);
    printf("La longueur de \'Un test\' est de : %d \n", len);
	printf("\n");
	
	
	printf("Test de \'strcat\'\n");
	char *d="Debut";
	char *f="Fin";
	char *cat=strcat(d,f);
	printf("Resultat de la concatenation de \"Debut\" avec \"Fin\" est: %s \n",cat);
	
	printf("Test de \'strncat\' (je l'ai implemente seulement car j'ai pas vu qu'il fallait pas)\n");
	char *catn=strncat(d,f,2);
	printf("Resultat de la concatenation de \"Debut\" avec \"Fin\", en prenant les 2 premier char de \"Fin\" est: %s \n",catn);
	printf("\n");
	
	printf("Test de \'strcasecmp\'\n");
	int co1=strcasecmp("a","b");
	printf("Resultat de la comparaison de \"a\" avec \"b\" : %d\n",co1);
	int co2=strcasecmp(d,f);
	printf("Resultat de la comparaison de \"Debut\" avec \"Fin\" : %d\n",co2);
	int co3=strcasecmp("TEST","Test");
	printf("Resultat de la comparaison de \"TEST\" avec \"test\" : %d\n",co3);
	printf("\n");
	
	return 0;
}
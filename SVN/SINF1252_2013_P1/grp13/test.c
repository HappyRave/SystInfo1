#include "bitstring.h"

#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD		   "\x1b[1m"
#define ANSI_BOLD_OFF	   "\x1b[21m"

#define CHECK printf(ANSI_COLOR_YELLOW " |"ANSI_COLOR_GREEN " √ "ANSI_COLOR_YELLOW "|\n" ANSI_COLOR_RESET)
#define FUNOK(X) printf("\n\tThe function "ANSI_COLOR_RED ANSI_BOLD X ANSI_COLOR_RESET ANSI_BOLD_OFF " successfully passed the tests\n")

#define LEN 24

#define MY_CU_ASSERT(value, args...) do { \
    int __b__ = (value); \
    if(! __b__) printf(args); \
    CU_ASSERT_FATAL(__b__); \
} while(0)

void aperture();
void cake();
/* Ptr vers les bitstring employés pour les tests */
static bitstring_t * b;

/* The suite initialization function.
* Returns zero on success, non-zero otherwise.
*/
int init_suite1(void) {
    /* code pour l'init de la suite de tests, alloc ou autre... */
    b = NULL;
    return 0; /* elle marche tjrs */
}

/* The suite cleanup function.
* Returns zero on success, non-zero otherwise.
*/
int clean_suite1(void) {
    /* nettoyage des données employées par la suite : free, etc... */
    if(b)
        bitstring_free(b);
    return 0;
}

/* Teste bitstring_alloc() */
void testAlloc() {
    int i;
    b = bitstring_alloc(LEN);

    printf("\n");
    /* verif mémoire allouée */
    MY_CU_ASSERT(b != NULL,
                 "Erreur : Le ptr bitstring 'b' ne pointe vers aucune addresse"
                 " mémoire. Lui avez-vous alloué de la mémoire (malloc()) ?"
                 " Renvoyez-vous le bon pointeur ?\n");

    MY_CU_ASSERT(bitstring_len(b) == LEN,
                 "Erreur : Le bitstring n'a pas la bonne longueur (en bits)."
                 " Avez-vous pensé à l'affecter à une variable de la structure après avoir"
                 " alloué la mémoire ? Il se peut que ce soit votre fonction 'bitstring_len()'"
                 " qui n'est pas correcte.\n");

    for (i = 0; i < LEN; i++) {
        MY_CU_ASSERT(bitstring_get(b, i) == 0,
                     "Erreur : Au moins un des bits du bitstring ne vaut pas 0. Assurez-vous"
                     " que l'espace mémoire pour les données du bitstring soit bien mis"
                     " à 0 après l'allocation. De plus, est-ce que bitstring_get est correcte?\n");
    }

    bitstring_free(b);
    b = bitstring_alloc(18); /* 18 n'est pas multiple de 8 */

    MY_CU_ASSERT(b == NULL,
                 "Erreur : Le ptr bitstring 'b' devrait être NULL car on a essayé de l'allouer"
                 " pour une longueur de 18 bits, ce qui n'est pas multiple de 8."
                 " Avez-vous pensé à vérifier que la longueur passée en paramètre"
                 " était bien un multiple de 8 avant d'allouer de la mémoire ?"
                 " Renvoyez-vous bien le ptr NULL si ce n'est pas un multiple de 8 ?\n");

    bitstring_free(b);
    b = NULL;
    FUNOK("'bitstring_alloc()'");
	printf("\tBistring_alloc() seems fully functional");
	CHECK;
    printf("\t");
}

/* Teste bitstring_alloc_from_int */
void testAllocInt() {
	int i;
	unsigned int testInt = 42*1337*7;
	b = bitstring_alloc_from_int(testInt);

	printf("\n");
	/* verif mémoire allouée */
	MY_CU_ASSERT(b != NULL,
			 "Erreur : Le ptr bitstring 'b' ne pointe vers aucune addresse"
			 " mémoire. Lui avez-vous alloué de la mémoire (malloc()) ?"
			 " Renvoyez-vous le bon pointeur ?\n");

	MY_CU_ASSERT(bitstring_len(b) == (int)(sizeof(int)*8),
			 "Erreur : Le bitstring n'a pas la bonne longueur (en bits)."
			 " Avez-vous pensé à l'affecter à une variable de la structure après avoir"
			 " alloué la mémoire ? Il se peut que ce soit votre fonction 'bitstring_len()'"
			 " qui n'est pas correcte.\n");
	
	for (i=0; i<(int)(sizeof(int)*8); i++) {
		bit_t testBit = ((testInt >> i) & 1);
		MY_CU_ASSERT(bitstring_get(b, i) == testBit,
                     "Erreur : Au moins un des bits du bitstring ne correspond pas au int %d. Assurez-vous"
                     " que l'espace mémoire pour les données du bitstring soit bien mis,"
                     " et que la fonction alloc_from_int soit correct\n", testInt);
	}
	bitstring_free(b);
	
	// test valeur particulière: 0
	testInt = 0;
	b = bitstring_alloc_from_int(testInt);
	
	/* verif mémoire allouée */
	MY_CU_ASSERT(b != NULL,
				 "Erreur : Le ptr bitstring 'b' ne pointe vers aucune addresse"
				 " mémoire. Lui avez-vous alloué de la mémoire (malloc()) ?"
				 " Renvoyez-vous le bon pointeur ?\n");
	
	MY_CU_ASSERT(bitstring_len(b) == (int)(sizeof(int)*8),
				 "Erreur : Le bitstring n'a pas la bonne longueur (en bits)."
				 " Avez-vous pensé à l'affecter à une variable de la structure après avoir"
				 " alloué la mémoire ? Il se peut que ce soit votre fonction 'bitstring_len()'"
				 " qui n'est pas correcte.\n");
	
	for (i=0; i<(int)(sizeof(int)*8); i++) {
		bit_t testBit = ((testInt >> i) & 1);
		MY_CU_ASSERT(bitstring_get(b, i) == testBit,
                     "Erreur : Au moins un des bits du bitstring ne correspond pas au int %d. Assurez-vous"
                     " que l'espace mémoire pour les données du bitstring soit bien mis,"
                     " et que la fonction alloc_from_int soit correct\n", testInt);
	}
	bitstring_free(b);
	
	// test valeur particulière: gros int
	testInt = 3405643994u; // gros int
	b = bitstring_alloc_from_int(testInt);
	
	/* verif mémoire allouée */
	MY_CU_ASSERT(b != NULL,
				 "Erreur : Le ptr bitstring 'b' ne pointe vers aucune addresse"
				 " mémoire. Lui avez-vous alloué de la mémoire (malloc()) ?"
				 " Renvoyez-vous le bon pointeur ?\n");
	
	MY_CU_ASSERT(bitstring_len(b) == (int)(sizeof(int)*8),
				 "Erreur : Le bitstring n'a pas la bonne longueur (en bits)."
				 " Avez-vous pensé à l'affecter à une variable de la structure après avoir"
				 " alloué la mémoire ? Il se peut que ce soit votre fonction 'bitstring_len()'"
				 " qui n'est pas correcte.\n");
	
	for (i=0; i<(int)(sizeof(int)*8); i++) {
		bit_t testBit = ((testInt >> i) & 1);
		MY_CU_ASSERT(bitstring_get(b, i) == testBit,
                     "Erreur : Au moins un des bits du bitstring ne correspond pas au int %d. Assurez-vous"
                     " que l'espace mémoire pour les données du bitstring soit bien mis,"
                     " et que la fonction alloc_from_int soit correct\n", testInt);
	}
	bitstring_free(b);
	
	// test valeur particulière: MAX_INT
	testInt = 4294967295u; //MAX_UNINT
	b = bitstring_alloc_from_int(testInt);
	
	/* verif mémoire allouée */
	MY_CU_ASSERT(b != NULL,
				 "Erreur : Le ptr bitstring 'b' ne pointe vers aucune addresse"
				 " mémoire. Lui avez-vous alloué de la mémoire (malloc()) ?"
				 " Renvoyez-vous le bon pointeur ?\n");
	
	MY_CU_ASSERT(bitstring_len(b) == (int)(sizeof(int)*8),
				 "Erreur : Le bitstring n'a pas la bonne longueur (en bits)."
				 " Avez-vous pensé à l'affecter à une variable de la structure après avoir"
				 " alloué la mémoire ? Il se peut que ce soit votre fonction 'bitstring_len()'"
				 " qui n'est pas correcte.\n");
	
	for (i=0; i<(int)(sizeof(int)*8); i++) {
		bit_t testBit = ((testInt >> i) & 1);
		MY_CU_ASSERT(bitstring_get(b, i) == testBit,
                     "Erreur : Au moins un des bits du bitstring ne correspond pas au int %d. Assurez-vous"
                     " que l'espace mémoire pour les données du bitstring soit bien mis,"
                     " et que la fonction alloc_from_int soit correct\n", testInt);
	}
	bitstring_free(b);

	
    b = NULL;
    FUNOK("'bitstring_alloc_from_int()'");
	printf("\tBistring_alloc_from_int() seems fully functional");
	CHECK;
    printf("\t");
}


/* Teste bitstring_set() et bitstring_get() */
void testSet() {
    int i;
    int testbit = 19;

    b = bitstring_alloc(LEN);

    printf("\n");

    printf("\tSetting bit %d to 1\n", testbit);
    bitstring_set(b, testbit, 1); /*on met le bit testbit à 1*/

    /* vérifie que tous les bits sont à 0 (ignore le testbit) */
    for(i = 0; i < LEN; ++i) {
        if(i == testbit)
                continue;
        MY_CU_ASSERT(bitstring_get(b, i) == 0,
                     "Erreur : Impossible de tester la fonction 'bitstring_set()' car"
                     " votre fonction 'bitstring_get()' ou 'bitstring_alloc()' n'est pas correcte.\n");
    }

    /* vérifie que le bit testbit est bien à 1 */
    MY_CU_ASSERT(bitstring_get(b, testbit) == 1,
                 "Erreur : Tous les bits semblent être à 0 dans votre bitstring alors"
                 " que le n°%d (0 étant le premier) devrait être à 1. Le problème"
                 " peut venir de 'bitstring_set()', mais aussi de 'bitstring_get()'.\n",
                 testbit);


    /* on assigne un autre bit dans le même octet pour voir si ça n'efface pas l'autre */
    printf("\tSetting bit %d to 1\n", testbit+2);
    bitstring_set(b, testbit + 2, 1);

    /* revérifie tous les bits */
    /* intéressant uniquement si les étudiants codent 8 bits par octet,
mais ne devrait pas trigger dans le cas contraire */
    for(i = 0; i < LEN; ++i) {
        if(i == testbit || i == testbit + 2) {
            MY_CU_ASSERT(bitstring_get(b, i) == 1,
                         "Erreur : Affecter un seul bit fonctionne correctement, mais en"
                         " affecter un second dans le même octet semble effacer le premier (ou ne"
                         " pas affecter correctement le second). Vérifiez que vous ne mettez pas"
                         " tout l'octet à 0 avant d'y set un bit. (Il se peut aussi que ce soit du"
                         " à 'bitstring_get()')\n");
        } else {
            MY_CU_ASSERT(bitstring_get(b, i) == 0,
                         "Erreur : Affecter un seul bit fonctionne correctement, mais en"
                         " affecter un second dans le même octet altère des bits qui ne devraient pas"
                         " l'être. (Il se peut aussi que ce soit dû à 'bitstring_get()')\n");
        }
    }


    /* Il faudrait éventuellement penser à ajouter un test sur le premier et dernier bit du bitstring,
pour voir s'il n'y a pas d'effet de bord : les numéros de bits allant de 0 à bitstring_len() − 1 */
	/* C'est fait! */
	int last = LEN-1;
	int first = 0;
	printf("\tSetting bit %d to 1\n", last);
	bitstring_set(b,last,1);
		MY_CU_ASSERT(bitstring_get(b, last) == 1,
				 "Erreur : Impossible de tester la fonction 'bitstring_set()' car"
				 " votre fonction 'bitstring_get()' ou 'bitstring_alloc()' n'est pas correcte.\n"
				 "Pensez aussi à vérifier si n=31 correspond bien au 32eme bit\n");
	
	printf("\tSetting bit %d to 1\n", first);
	bitstring_set(b,first,1);
	MY_CU_ASSERT(bitstring_get(b, first) == 1,
				"Erreur : Impossible de tester la fonction 'bitstring_set()' car"
				" votre fonction 'bitstring_get()' ou 'bitstring_alloc()' n'est pas correcte.\n"
				"Pensez aussi à vérifier si n=0 correspond bien au 1er bit\n");

	bitstring_free(b);
	b = NULL;
    /* succès */
    printf("\n\tThe functions "ANSI_COLOR_RED ANSI_BOLD"'bitstring_set() and get()'"ANSI_COLOR_RESET ANSI_BOLD_OFF" successfully passed the tests\n");
	printf("\tBistring_set() and get() seem fully functional");
	CHECK;
    printf("\t");
}

//test de rotate : 

void testRotate()
{
	printf("\b");
	unsigned int gdInt=3405643994u;
	b = bitstring_alloc_from_int(gdInt);
	
        //essai de bitstring_rotate(b,0) avec un grand entier non-signé

        bitstring_rotate(b,0);	

	int i = 0;
	
	for(i=0;i<(bitstring_len(b));i++)//on va vérifier chaque bit
	{
	  bit_t our = bitstring_get(b,i);
	  
      
	  MY_CU_ASSERT( ((gdInt>>i) & 1) ==  our,
			"Erreur : les bits ont changé dans le cas bitstring_rotate(b,0) pour le gdInt, alors qu'ils n'auraient pas dû\n. L'erreur est pour ce i = %d\n",i
				 );
	}

	bitstring_free(b);

	//essai de bitstring_rotate(b,0) avec un petit entier non-signé
	
	unsigned int pttInt = 42u; 
	b = bitstring_alloc_from_int(pttInt);

	bitstring_rotate(b,0);	

	i = 0;
	
	for(i=0;i<(bitstring_len(b));i++)//on va vérifier chaque bit
	{
	  bit_t our = bitstring_get(b,i);
	  
      
	  MY_CU_ASSERT( ((pttInt>>i) & 1) ==  our,
			"Erreur : les bits ont changé dans le cas bitstring_rotate(b,0) pour le pttInt, alors qu'ils n'auraient pas dû\n. L'erreur est pour ce i = %d\n",i
				 );
	}

	bitstring_free(b);

	
	
	
    FUNOK("'bitstring_rotate()'");
	printf("\tBistring_rotate() seems fully functional");
	CHECK;
    printf("\t");
}



/* teste bitstring_print() */
void testPrint()
{
	printf("\n");
	int i;
	char *c;
	char *hex;
	int ret;
	for (i=0; i<256; i++) { // nous testons TOUS les nombre entre 00 et FF, pour être sûr que ça marche dans tous les cas
		b = bitstring_alloc_from_int(i);
		c = (char*)malloc(sizeof(char)*3);
		ret = bitstring_print(b,c,3);
		hex = (char*)malloc(sizeof(char)*3);
		sprintf(hex,"%X",i);
		MY_CU_ASSERT(ret != -1 && ret <= 3 && ret >=2,
					 "Erreur : bitstring_print() n'as pas reussi à caser un hexa de 2 (int == %d && ret == %2d) chhiffre dans un buffer"
					 " de taille 3. Verfier les conditions dans bitstring_print()", i,ret);
		MY_CU_ASSERT(*c == *hex,
					 "Erreur : bitstring_print() n'e pas bien converti %d en hexadecimal",i);
		free(hex);
		free(c);
		bitstring_free(b);
		b=NULL;
	}
	
	/* On test si bitsring_print() fonctionne avec de très gros nombre, et dans les cas limites comme int = 0 et
	 int = INT_MAX */
	unsigned int testInt=0;
	b = bitstring_alloc_from_int(testInt);
	c = (char*)malloc(sizeof(char)*33);
	ret = bitstring_print(b,c,33);
	hex = (char*)malloc(sizeof(char)*33);
	sprintf(hex,"%X",testInt);
	printf("\t");
	printf("%u ([unsigned] int) == %X (natif hex) == %s (bitstring_print())\n",testInt,testInt,c);
	printf("\t");
	MY_CU_ASSERT(ret != -1 && ret == 2,
				 "Erreur : bitstring_print() n'as pas reussi à caser un hexa de 1 chhiffre dans un buffer"
				 " de taille 3. Verfier les conditions dans bitstring_print()");
	MY_CU_ASSERT(*c == *hex,
				 "Erreur : bitstring_print() n'e pas bien converti %d en hexadecimal",testInt);
	free(hex);
	free(c);
	bitstring_free(b);
	
	testInt=4294967295u; //MAX_UNINT
	b = bitstring_alloc_from_int(testInt);
	MY_CU_ASSERT(b != NULL,
				 "bitstrind_alloc_from_int() n'as pa ssu initialiser %d\n",testInt);
	c = (char*)malloc(sizeof(char)*33);
	ret = bitstring_print(b,c,33);
	hex = (char*)malloc(sizeof(char)*33);
	sprintf(hex,"%X",testInt);
	printf("%u ([unsigned] int) == %X (natif hex) == %s (bitstring_print())\n",testInt,testInt,c);
	printf("\t");
	MY_CU_ASSERT(ret != -1 && ret == 9,
				 "Erreur : bitstring_print() n'as pas reussi à caser un hexa de 32 chhiffre dans un buffer"
				 " de taille 33. Verfier les conditions dans bitstring_print()");
	MY_CU_ASSERT(*c == *hex,
				 "Erreur : bitstring_print() n'e pas bien converti %d (max int) en hexadecimal",testInt);
	free(hex);
	free(c);
	bitstring_free(b);
	
	testInt=3405643994u; // gros int
	b = bitstring_alloc_from_int(testInt);
	c = (char*)malloc(sizeof(char)*33);
	ret = bitstring_print(b,c,33);
	hex = (char*)malloc(sizeof(char)*33);
	sprintf(hex,"%X",testInt);
	printf("%u ([unsigned] int) == %X (natif hex) == %s (bitstring_print())\n",testInt,testInt,c);
	printf("\t");
	MY_CU_ASSERT(ret != -1 && ret == 9,
				 "Erreur : bitstring_print() n'as pas reussi à caser un hexa de 32 chhiffre dans un buffer"
				 " de taille 33. Verfier les conditions dans bitstring_print()");
	MY_CU_ASSERT(*c == *hex,
				 "Erreur : bitstring_print() n'a pas bien converti %d (gros int) en hexadecimal",testInt);
	free(hex);
	free(c);
	bitstring_free(b);
	
	// test des valeurs de renvoi
	
	testInt=255; // en hexa: 2 chiffre, donc buf de 3 suffit
	b = bitstring_alloc_from_int(testInt);
	c = (char*)malloc(sizeof(char)*3);
	ret = bitstring_print(b,c,3);
	MY_CU_ASSERT(ret != -1 && ret == 3,
				 "Erreur : bitstring_print() n'as pas reussi à caser un hexa de 2 chhiffre dans un buffer"
				 " de taille 3. Verfier les conditions dans bitstring_print()");
	free(c);
	c = (char*)malloc(sizeof(char)*2);//buffer trop petit
		ret = bitstring_print(b,c,2);
	MY_CU_ASSERT(ret == -1,
				 "Erreur : bitstring_print() as reussi à caser un hexa de 2 chhiffre dans un buffer"
				 " de taille 2, ce qui avec le char de terminaison ets imposible. Verfier les conditions dans bitstring_print()");
	free(c);
	bitstring_free(b);

	
    b = NULL;
    FUNOK("'bitstring_print()'");
	printf("\tBistring_print() seems fully functional");
	CHECK;
    printf("\t");
}

/* The main() function for setting up and running the tests.
* Returns a CUE_SUCCESS on successful running, another
* CUnit error code on failure.
*/
int main()
{
	aperture();
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if(CUE_SUCCESS != CU_initialize_registry())
       return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite de tests pour bitstring", init_suite1, clean_suite1);
    if(NULL == pSuite) {
       CU_cleanup_registry();
       return CU_get_error();
    }

    /* add the tests to the suite */
    /* NOTE - ORDER IS IMPORTANT - first fct added = first to be run */
    if(NULL == CU_add_test(pSuite, "test de 'bitstring_alloc()'", testAlloc) ||
       NULL == CU_add_test(pSuite, "test de 'bitstring_alloc_from_int()'", testAllocInt) || NULL == CU_add_test(pSuite, "test de 'bitstring_set()'", testSet) || NULL == CU_add_test(pSuite, "test de 'bitstring_set()'", testRotate) || NULL == CU_add_test(pSuite, "test de 'bitstring_print()'", testPrint)) {
       CU_cleanup_registry();
       return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_SILENT);
    CU_basic_run_tests();
    CU_cleanup_registry();
    
	cake();
    return CU_get_error();
}

void aperture()
{
	printf(ANSI_COLOR_CYAN ANSI_BOLD"                                         \n\t");
	printf("                  .,-:;//;:=,\n\t");
	printf("              . :H@@@MM@M#H/.,+%%;,\n\t");
	printf("           ,/X+ +M@@M@MM%%=,-%%HMMM@X/,\n\t");
	printf("         -+@MM; $M@@MH+-,;XMMMM@MMMM@+-\n\t");
	printf("        ;@M@@M- XM@X;. -+XXXXXHHH@M@M#@/.\n\t");
	printf("      ,%%MM@@MH ,@%%=            .---=-=:=,.\n\t");
	printf("      =@#@@@MX .,              -%%HX$$%%%%%%+;\n\t");
	printf("     =-./@M@M$                  .;@MMMM@MM:\n\t");
	printf("     X@/ -$MM/                    .+MM@@@M$\n\t");
	printf("    ,@M@H: :@:                    . =X#@@@@-\n\t");
	printf("    ,@@@MMX, .                    /H- ;@M@M=\n\t");
	printf("    .H@@@@M@+,                    %%MM+..%%#$.\n\t");
	printf("     /MMMM@MMH/.                  XM@MH; =;\n\t");
	printf("      /%%+%%$XHH@$=              , .H@@@@MX,\n\t");
	printf("       .=--------.           -%%H.,@@@@@MX,\n\t");
	printf("       .%%MM@@@HHHXX$$$%%+- .:$MMX =M@@MM%%.\n\t");
	printf("         =XMMM@MM@MM#H;,-+HMM@M+ /MMMX=\n\t");
	printf("           =%%@M@M#@$-.=$@MM@@@M; %%M%%=\n\t");
	printf("    	     ,:+$+-,/H#MMMMMMM@= =,\n\t");
	printf("                   =++%%%%%%%%+/:-.\n\t");
	printf("    ........................................\n\t");
	printf("    ....... Aperture Laboratories(R)  ......\n\t");
	printf("    ........................................\n\t");
	printf("\n\t");
	printf("\n\tWelcome to the Aperture Science Enrichment Center\n\t" ANSI_BOLD_OFF ANSI_COLOR_RESET);
	printf("\n\t");
	printf(ANSI_COLOR_YELLOW "We will now proceed to a meticulously composed battery of tests" ANSI_COLOR_RESET);
}

void cake()
{
	printf( ANSI_COLOR_RED"\n");
	printf( ANSI_BOLD "\tCongratulation, you brilliantly passed all our extremely difficult tests !\t\n");
	printf("\n");
	printf("\tHere, take this piece of delicious cake.\n");
	printf("\t\n");
	printf("\t               "ANSI_COLOR_YELLOW",:/+/-\t\n"ANSI_COLOR_RED);
	printf("\t               "ANSI_COLOR_YELLOW"/M/              .,-=;//;-\t\n"ANSI_COLOR_RED);
	printf("\t          "ANSI_COLOR_YELLOW".:/= ;MH/,    ,=/+$$XH@MM#@:\t\n"ANSI_COLOR_RED);
	printf("\t         "ANSI_COLOR_YELLOW"-$##@+$###@H@MMM#######H:.  "ANSI_COLOR_RED"  -/H#\t\n");
	printf("\t    "ANSI_COLOR_YELLOW".,H@H@ X######@ -H#####@+-  "ANSI_COLOR_RED"   -+H###@X\t\n");
	printf("\t    "ANSI_COLOR_YELLOW" .,@##H;      +XM##M/,  "ANSI_COLOR_RED"   =$@###@X;-\t\n");
	printf("\t   X$- "ANSI_COLOR_YELLOW" :M##########$. "ANSI_COLOR_RED"   .:$M###@$:\t\n");
	printf("\t   M##H, "ANSI_COLOR_YELLOW"  +H@@@$/-. "ANSI_COLOR_RED" ,;$M###@$,          -\t\n");
	printf("\t   M####M=,,---,.-$$H####M$:          ,+@##\t\n");
	printf("\t   @##################@/.         :$H##@$-\t\n");
	printf("\t   M###############H,         ;HM##M$=\t\n");
	printf("\t   #################.    .=$M##M$=\t\n");
	printf("\t   ################H..;XM##M$=          .:+\t\n");
	printf("\t   M###################@$=           =+@MH$\t\n");
	printf("\t   @################M/.          =+H#X$=\t\n");
	printf("\t   =+M##############M,       -/X#X+;.\t\n");
	printf("\t     .;XM##########H=    ,/X#H+:,\t\n");
	printf("\t        .=+HM######M+/+HM@+=.\t\n");
	printf("\t            ,:/$XM####H/.\t\n");
	printf("\t                 ,.:=-.\t\n" ANSI_BOLD_OFF);
	printf("\t\n"ANSI_COLOR_RESET);
}

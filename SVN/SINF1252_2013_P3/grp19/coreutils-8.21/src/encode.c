#include <config.h>
#include "system.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> 

/* The official name of this program (e.g., no 'g' prefix).  */
#define PROGRAM_NAME "encode"

#define AUTHORS \
  proper_name_utf8 ("de Mol Maxime"), \
  proper_name ("de Bodt Cyril")

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Paramètres %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

#define LENGTH 128

static int taille_max; //taille maximale mappée en mémoire

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Données %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static char *input; // path de l'input

static char *output;//path de l'output

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% File-Descriptor %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static int fd_in; //file descriptor de l'entrée

static int fd_out; //file descriptor de la sortie

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Données sur les fichiers %%%%%%%%%%%%%%%%%%%%%%%%%%%%%

static struct stat input_stat; //donnée sur l'input

void *src_input; //adresse de la partie de l'input couramment mappée en mémoire

void *src_output; // adresse de la partie de l'output couramment mappée en mémoire

void *src_output_iter_prec; // adresse de la partie de l'output que l'on remappe en mémoire pour faire des modifications sur la partie mappée dans l'itération précédente (sur base de la nouvelle partie couramment mappée)

int taille_to_map_iter_prec; // taille à remapper de la partie du fichier traitée précédemment

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Données sur le nombre d'itération (dans le traitement de l'input en plusieurs map) %%%%%%%%%%%%%%%

static int iteration; //n° correspondant à l'itération courante dans le traitement de l'input mappé en plusieurs fois.

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Données sur les derniers bytes de l'itération précédente %%%%%%%%%%%%%%%%%%%%%%%%%%

static unsigned char last_byte_iter_prec; //dernier byte dans la partie de l'input traitée dans l'itération précédente (car on map l'input en plusieurs fois)

static int nb_1_successif_iter_prec; //nb de length=1 successifs qui finissaient l'itération précédente dans l'input

static int nb_repetition_last_byte_iter_prec; //nombre de répétition du dernier byte précédent

static off_t offset_last_length_iter_prec; //offset du dernier length dans l'output 

static off_t offset_page_output_iter_prec; //offset, correspondant à la page de début, de l'itération précédente

static int check_length_last_byte; //vaut 1 s'il faut prendre en compte le dernier length du mapping précédent dans le cas d'une succession de length valant 1 coupée par un mapping.

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Données sur l'itération courante %%%%%%%%%%%%%%%%%%%%%%%%

static int nb_1_successif; //donne le nombre de 1 successif dans l'itération courante

static int taille_out; //taille nécessaire dans l'output vis-à-vis de la partie de l'input couramment mappée en mémoire

static int taille_to_map_output; //taille à mappé de l'output dans l'itération courant

static unsigned char* current_mmap_output; //position du byte courant dans le mapping de l'output

static int taille_output_tot; //taille totale de l'output

static int offset_in_mapped_output; //offset dans l'output couramment mappé en mémoire

static off_t offset_input; // offset dans l'input (multiple de la taille des pages)

static off_t offset_output; // offset dans l'output (multiple de la taille des pages)

void
usage (int status)
{
  if (status != EXIT_SUCCESS)
    emit_try_help ();
  else
    {
      printf (_("\
Usage: %s [FILE]... [FILE]...\n\
"),
              PROGRAM_NAME);
      fputs (_("\
Compress the file given by first argument using the run length encoding algorythm and write it in file given by second argument.\n\
If the output file does'nt exist, it will be created. In case of errors errno is updated \n\
"), stdout);
		printf (_("\
\n\
Options:\n\
%s takes no options\n\
"),
				PROGRAM_NAME);
      printf (_("\
\n\
Examples:\n\
  %s f g  Compress the content of f and write the result in g.\n\
"),
              PROGRAM_NAME);
      emit_ancillary_info ();
    }
  exit (status);
}

/*
gère le cas d'erreur
*/
void error(int e, char* msg){ //FONCTIONNE
	fprintf(stderr,"%s a renvoyé %d. Message d'erreur : %s\n",msg,e,strerror(errno));
	exit(-1);
}

/*
ferme un ficher sur base du file descriptor
*/
void close_file(int fd){ //FONCTIONNE
	int err = close(fd);
	if(err!=0){
		error(err,"close");
	}
}
/*
ferme tous nos fichiers
*/
void close_all_file(){ //FONCTIONNE
	close_file(fd_in);
	close_file(fd_out);
}

/*
ouvre les fichiers et initialise les structures stat
*/
void open_file(){
	// %%%%%%%%%%%%%%%% Overture des fichiers %%%%%%%%%%%%%%%%%%%

	fd_in = open(input,O_RDONLY);
	if(fd_in==-1){
		error(fd_in,"open de l'input");
	}

	int err = fstat(fd_in,&input_stat);
	if (err < 0) {
		close_file(fd_in);
   		error(err,"fstat de l'input");
	}
	
	// ON SUPPOSE QUE LE CONTENU DE OUTPUT EST VIDE !
	fd_out = open(output,O_RDWR|O_CREAT,input_stat.st_mode);//si le fichier n'existe pas, on lui donne les mêmes permissions que celles de l'input (on met RDWR pour ne pas avoir de problème avec le mmap)
	if(fd_out==-1){
		close_file(fd_in);
		error(fd_out,"open de l'output");
	}
}

/*
renvoit le minimum entre deux nombres
*/
int min(int a, int b){
	if(a>b){
		return b;
	}
	return a;
}

/*
fonction qui compare deux bytes : elle renvoit 0 s'il sont égaux, un autre chiffre
sinon
ATTENTION : la méthode pr comparer les bytes marche bien pr les char, pas pour les 
int...
*/

int compare_byte(const void *byte1, const void *byte2){ // FONCTIONNE
	return memcmp(byte1, byte2, 1);
}

/*
renvoit le nombre de byte identique à byte et qui suivent directement byte
*/
int give_nb_same_byte(void *byte){ // FONCTIONNE 
	int res = 0;
	int pass = 1;
	while(pass==1) {
		if(compare_byte(byte,byte+1)==0){
			res++;
			byte++;
		}
		else{
			pass = 0;
		}
	}
	return (res);
}

/*
Cette fonction reçoit en paramètre double pointeur vers un byte,
va utiliser give_nb_same_byte et mettre à jour le pointeur vers le byte (c'est pour cela qu'il faut un double pointeur)
Valeur de retour : le nombre de fois que le byte est répété (qu'il soit plus grand que LENGTH ou non)
*/

int use_give_nb_same_byte(void **byte){ // FONCTIONNE
	int res = give_nb_same_byte(*byte);
	*byte = *byte+res+1; //+1 car le res renvoit 0 quand le byte n'est pas répété.
	return (res+1);
}

/*
renvoit le nombre de fois que la fonction est plus grande que length + 1
*/
int number_time_bigger_then_length(int i){ // FONCTIONNE
	return(1+(i-1)/LENGTH);
} 

void treate_big_nb_1_successif(){
	while(nb_1_successif>=LENGTH){
		taille_out = taille_out + 1+LENGTH-1;
		nb_1_successif = nb_1_successif - (LENGTH-1);
	}
	taille_out = taille_out + 1 + nb_1_successif;
	nb_1_successif = 0;
}

/*
traite nb_1_successif dans le cas du calcul de taille_out.
Rajout la valeur def par défaut à taille_out
*/
void treatement_nb_1_successif(int def){ // FONCTIONNE
	taille_out = taille_out + def;
	if(check_length_last_byte!=1){
		if(nb_1_successif != 0){ 
			treate_big_nb_1_successif();
		}
	}
	else{ //il faut prendre en compte la fin de la version précédente mappée en mémoire
		int precedent = nb_1_successif_iter_prec%(LENGTH-1);
		if(nb_1_successif_iter_prec == 0){
			precedent = 0;
		}
		else if(precedent==0){
			precedent = LENGTH-1;
		}
		int tmp = (LENGTH-1-precedent) - nb_1_successif;
		if(tmp>=0){
			taille_out += nb_1_successif; //ATTENTION : PEUT-ÊTRE REMETTRE NB_1_SUCCESSIF supérieur à 0.
			nb_1_successif = 0;
		}
		else{
			nb_1_successif = nb_1_successif - (LENGTH-1-precedent);
			treate_big_nb_1_successif();
		}
		check_length_last_byte = 0;
	}
}

/*
fonction qui renvoit 1 si le dernier byte de la séquence précédente est égal à celui passé en argument
0 sinon
*/
int same_byte(char courant){
	if((unsigned char)courant == last_byte_iter_prec){
		return 1;
	}
	return 0;
}

/*
fonction qui renvoit 1 si le dernier byte de la séquence précédente résultait d'une succession de length 1
0 sinon
*/	
int last_byte_succession_1(){
	if(nb_repetition_last_byte_iter_prec==1){
		return 1;
	}
	return 0;
}

/*
fonction qui renvoit 1 si le dernier byte  de la séquence précédente se répétait
0 sinon
*/
int last_byte_repeated(){
	if(nb_repetition_last_byte_iter_prec>1){
		return 1;
	}
	return 0;
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%% Fonctions qui comptent la taille nécessaire dans l'output pour la nouvelle zone de l'input mappée en mémoire %%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
fonction qui va renvoyer un entier pour modifier num>LENGTH dans la fonction count_length_output 
pour gérer le cas où le 1e byte au début de la nouvelle portion de l'output est le même que
celui à la fin.
*/
int treatement_num_bigger_length(char courant,int num){ // NORMALEMENT OK !!!
	if(same_byte(courant)==1){
		if(last_byte_repeated()==1){
			num = num - (LENGTH-nb_repetition_last_byte_iter_prec);
			nb_repetition_last_byte_iter_prec = LENGTH;
		}
		else if(last_byte_succession_1()==1){
			num = -1;// cas spécial à traiter dans treat_case_num_bigger_length
		}
	}
	return num;
}

/*
même principe que la fonction ci-dessus, mais dans le cas num<=LENGTH
*/
int treatement_num(char courant,int num){
	if(same_byte(courant)==1){
		if(last_byte_repeated()==1){
			if((LENGTH-nb_repetition_last_byte_iter_prec)>=num){
				num = 0;
				nb_repetition_last_byte_iter_prec += num;
			}
			else{
				nb_repetition_last_byte_iter_prec = LENGTH;
				num = num - (LENGTH-nb_repetition_last_byte_iter_prec);
			}
		}
		else if(last_byte_succession_1()==1){ 
			int tmp = nb_1_successif_iter_prec%(LENGTH-1);
			if((tmp>1)||(tmp==0)){
				num++;
				taille_out--; //car on récupère un byte dans le mapping précédent
			}
			else if(tmp==1){
				num=-1; //cas spécial à traiter dans treat_case_num_smaller_length
			}
		}
	}
	return num;
}

void treat_case_num_bigger_length(void*copi,int num, int i){
	int copy_num = 0;
	if((i==0)&&(iteration!=0)){
		copy_num = num;
		num = treatement_num_bigger_length(*((char*)(copi-1)),num);
		if(num<0){ //le cas où le dernier byte de la phase précédente a été répété qu'une fois avant la nouvelle phase, avec un length qui le précède
			int tmp = nb_1_successif_iter_prec%(LENGTH-1);	
			if(tmp==0||tmp>1){ //un length ne précède pas le dernier byte du mapping précédent
				num = copy_num+1;
				treatement_nb_1_successif(0);
				taille_out = taille_out + 2*(number_time_bigger_then_length(num))-1;
			}
			else{
				num = copy_num+1;
				treatement_nb_1_successif(0);
				taille_out = taille_out + 2*(number_time_bigger_then_length(num))-2;
			}
			num = copy_num+1; //un byte se rajoute
			taille_out--; //on soustrait une unité à taille_out car le length du dernier byte de la dernière phase est déjà présent
		}
		else{
			treatement_nb_1_successif(0);
			taille_out = taille_out + 2*(number_time_bigger_then_length(num));
		}
	}
	else{
		treatement_nb_1_successif(0);
		taille_out = taille_out + 2*(number_time_bigger_then_length(num));
	}
}

void treat_case_num_smaller_length(void* copi,int num, int i){
	int copy_num = 0;
	if((i==0)&&(iteration!=0)){
		copy_num = num;
		num = treatement_num(*((char*)(copi-1)),num);
		if(num==0){
			treatement_nb_1_successif(0);
		}
		else if(num>LENGTH){
			treatement_nb_1_successif(4);
		}
		else if(num<0){ //le dernier byte de la phase précédente n'est répété qu'une fois et a son length qui le précède
			if(copy_num == LENGTH){
				treatement_nb_1_successif(2);
			}
			else{
				treatement_nb_1_successif(0);
			}
		}
		else{
			treatement_nb_1_successif(2);
		}
	}
	else {
		treatement_nb_1_successif(2);
	}
}

void treat_case_num_1_length(void* copi,int num, int i){
	if((i==0)&&(iteration!=0)){
		char courant = *((char*)(copi-1));
		if(same_byte(courant)==1){ 
			if(last_byte_repeated()==1){
				if(nb_repetition_last_byte_iter_prec == LENGTH){
					taille_out += 2;
				}//dans l'autre cas, il ne faut rien faire car il y a encore assez de place dans le dernier length du map précédant
			}
			else if(last_byte_succession_1()){
				int tmp = nb_1_successif_iter_prec%(LENGTH-1);
				if((tmp>1)||(tmp==0)){
					taille_out++; //on rajoute une place pour un length
				}
				// dans l'autre cas, =1, il ne faut rien faire car il reste assez de place dans le denier length du map précédant que pour stocker le byte courant
			}
		}
		else{ 
			if(last_byte_repeated()==1){
				nb_1_successif++;
			}
			else if(last_byte_succession_1()){
				nb_1_successif++;
				check_length_last_byte = 1;
			}
		}
	}
	else{
		nb_1_successif++;
	}
}

/*
Calcul taille_out pour une partie mappée de l'input
*/
void count_length_output(void *src_input,int len){ 
	void *copi = src_input;
	taille_out = 0;
	int num = 0;
	int i = 0;
	while(copi<=(src_input+len-1)){
		num = use_give_nb_same_byte(&copi);
		if(num>LENGTH){
			treat_case_num_bigger_length(copi,num,i);
		}
		else if(num!=1){
			treat_case_num_smaller_length(copi,num,i); 
		}
		else if(num==1){
			treat_case_num_1_length(copi,num,i);
		}
		if(copi == src_input+len){ //cas de la dernière itération dans la zone couramment mappée en mémoire (on rajoute une adresse par rapport à la condition dans le while car le copi a été avancé par use_give_nb_same_byte)
			offset_page_output_iter_prec = offset_output;
			last_byte_iter_prec = *((unsigned char*)(copi-1));
			if(num%LENGTH==0){
				nb_repetition_last_byte_iter_prec = LENGTH;
			}
			else{
				nb_repetition_last_byte_iter_prec = num%LENGTH;
			}
			nb_1_successif_iter_prec = nb_1_successif; 
			treatement_nb_1_successif(0);
		}
		i++;
	}
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%% Fonction qui remplisse traite une zone mappée de l'output et de l'input %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
Remplit un byte de l'output et met à jour current_mmap_output
*/
void set_one_output(void* byte){
	*current_mmap_output = *((unsigned char*)byte);
	current_mmap_output++;
}

/*
Met à jour l'output dans le cas d'un byte répété dans l'input, mais répété moins que LENGTH fois
Paramètre : 
- byte est le byte de l'input qui se répète
- len est le nombre de fois qu'il se répète.
*/
void set_output_repeated_smaller_length(void* byte,int len){
	if(len!=0){
		if (len == 1) len = 129;
		if (len == 10) len = 1;
		unsigned int len2 = (unsigned int) len;
		void* v = (void*)(&len2);
		set_one_output(v);
		set_one_output(byte);	
	}
}

/*
Met à jour l'output dans le cas d'un byte répété dans l'input, qu'il soit répété plus ou moins que length fois
Paramètre : 
- byte est le byte de l'input qui se répète
- len est le nombre de fois qu'il se répète.
*/

void set_output_repeated(void* byte,int len){
	while(len>LENGTH){
		len = len - LENGTH;
		set_output_repeated_smaller_length(byte,LENGTH);
	}
	set_output_repeated_smaller_length(byte,len);
}

/*
Met à jour l'output (mais sans mettre le byte length) dans le cas d'une suite de byte qui ne se répètent qu'une fois chacun, mais où la suite de byte est de longueur plus petite que LENGTH-1.
Param : 
- byte est un pointeur vers le premier élément de la suite de byte qui se répète.
- len est la longueur de la suite de byte qui se répète
*/

void set_output_successif_1_without_length(void* byte, int len){
	if(len!=0){
		int i = 0;
		for(i=0;i<len;i++){
			set_one_output(byte+i);
		}
	}
}

/*
Met à jour l'output dans le cas d'une suite de byte qui ne se répètent qu'une fois chacun, mais où la suite de byte est de longueur 
plus petite que LENGTH-1
Param : 
- byte est un pointeur vers le premier élément de la suite de byte qui se répète.
- len est la longueur de la suite de byte qui se répète
*/

void set_output_successif_1_smaller_length(void* byte, int len){
	if(len>=LENGTH){
		len = len - LENGTH;
	}
	if(len!=0){
		len += LENGTH;
		unsigned int len2 = (unsigned int) len;
		void* v = (void*)(&len2);
		set_one_output(v);
		int i = 0;
		for(i=0;i<len-LENGTH;i++){
			set_one_output(byte+i);
		}
	}
}

/*
Met à jour l'output dans le cas d'une suite de byte qui ne se répètent qu'une fois chacun. La taille de la suite peut être quelconque
Param : 
- byte est un pointeur vers le premier élément de la suite de byte qui se répète.
- len est la longueur de la suite de byte qui se répète
*/

void set_output_successif_1(void* byte, int len){
	while(len>LENGTH-1){
		len = len - (LENGTH-1);
		set_output_successif_1_smaller_length(byte, LENGTH-1);
		byte = byte + LENGTH-1;
	}
	set_output_successif_1_smaller_length(byte, len);
}

/*
La fonction suivante renvoit un pointeur vers une partie de fichier output mappée en mémoire, partie de fichier qui correspond
à la fin de la partie mappée dans l'itération précédente. Plus précisément, cette fin de partie contient le dernier byte length avec le byte répété ou la succession de byte de length valant 1).
*/
void* give_ptr_debut_map_prec(){
	int taille_page = getpagesize();
	int offset_to_map = offset_last_length_iter_prec/taille_page;
	taille_to_map_iter_prec = 0;
	if(nb_repetition_last_byte_iter_prec==1){
		int L = nb_1_successif_iter_prec%(LENGTH-1);
		if(L==0){
			L = LENGTH-1;
		}
		taille_to_map_iter_prec = 1+L;
	}
	else if(nb_repetition_last_byte_iter_prec>1){
		taille_to_map_iter_prec = 2;
	}
	src_output_iter_prec = mmap(NULL,taille_to_map_output,PROT_WRITE|PROT_READ,MAP_SHARED,fd_out,offset_to_map);
	if(src_output==NULL){
		close_all_file();
		error(-1,"mmap sur l'output dans l'itération");
	}
	return src_output_iter_prec;
}


/*
cette fonction applique le unmap sur la partie du fichier de l'itération précédente que l'on a remappée.
*/
void unmap_map_iter_prec(){
	int err = munmap(src_output_iter_prec,taille_to_map_iter_prec);
	if(err<0) {
		close_all_file();
		error(err,"munmap sur la partie de l'output de l'itération précédente, que l'on remmape");
	}
}

/*
Fonction qui permet d'utiliser set_output_successif_1, mais au lieu de passer dans byte l'adresse du byte qui commence la suite de byte qui se succèdent avec des length valant 1, on donne l'adresse du byte qui suit le dernier byte de la suite de ceux qui se succèdent avec des length valant 1.
*/
void compress_nb_1_successif(void* byte){
	if((nb_1_successif!=0)&&(check_length_last_byte==0)){
		set_output_successif_1(byte-nb_1_successif+1,nb_1_successif);
	}
	else if((nb_1_successif!=0)&&(check_length_last_byte=1)){ //gère le cas d'une succession de byte de length valant 1 à la fin de l'outputà l'itération précédente
		int precedent = nb_1_successif_iter_prec%(LENGTH-1);
		if(nb_1_successif_iter_prec == 0){
			precedent = 0;
		}
		else if(precedent==0){
			precedent = LENGTH-1;
		}
		int tmp = (LENGTH-1-precedent) - nb_1_successif;
		if(tmp>=0){
			void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
			unsigned int tmp = (unsigned int) (LENGTH+precedent+nb_1_successif);
			*((unsigned char*)map_iter_prec) = (unsigned char)tmp;
			unmap_map_iter_prec();
			set_output_successif_1_without_length(byte-nb_1_successif,nb_1_successif); //on met la suite de la succession de byte mappée en mémoire dans la nouvelle partie mappée en mémoire, mais sans mettre de byte length car celui présent à la fin de la zone mappée précédemment s'en charge
		}
		else{
			void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
			unsigned int tmp = (unsigned int) (2*LENGTH-1);
			*((unsigned char*)map_iter_prec) = (unsigned char)tmp;
			unmap_map_iter_prec();
			set_output_successif_1_without_length(byte-nb_1_successif,(LENGTH-1-precedent));//on met la suite de la succession de byte mappée en mémoire dans la nouvelle partie mappée en mémoire, mais sans mettre de byte length car celui présent à la fin de la zone mappée précédemment s'en charge
			nb_1_successif = nb_1_successif - (LENGTH-1-precedent);
			set_output_successif_1(byte-nb_1_successif,nb_1_successif);
		}
		check_length_last_byte = 0;
	}
	nb_1_successif = 0;
}

/*
fonction qui gère l'interraction avec la fin de la zone précédemment mappée en mémoire, dans le cas d'une répétition d'un byte plus longue que LENGTH
*/
int compress_num_bigger_length(char courant,int num){ // NORMALEMENT OK !!!
	if(same_byte(courant)==1){
		if(last_byte_repeated()==1){
			num = num - (LENGTH-nb_repetition_last_byte_iter_prec);
			nb_repetition_last_byte_iter_prec = LENGTH;
			void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
			unsigned int tmp = (unsigned int) LENGTH;
			*((unsigned char*)map_iter_prec) = (unsigned char)tmp;
			unmap_map_iter_prec();
		}
		else if(last_byte_succession_1()==1){
			num = -1;// cas spécial à traiter dans treat_case_num_bigger_length
		}
	}
	return num;
}

void compress_case_num_bigger_length(void* copi,int num,int i){
	int copy_num = 0;
	compress_nb_1_successif(copi-num-1);
	if((i==0)&&(iteration!=0)){
		copy_num = num;
		num = compress_num_bigger_length(*((char*)(copi-1)),num);
		if(num<0){ //le cas où le dernier byte de la phase précédente a été répété qu'une fois avant la nouvelle phase, avec un length qui le précède		
			num = copy_num;
			num++;
			int tmp = nb_1_successif_iter_prec%(LENGTH-1);	
			if(tmp==0||tmp>1){ 
				void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
				unsigned int remplir = (unsigned int) LENGTH;
				*((unsigned char*)map_iter_prec) = (unsigned char)(((unsigned int)(*((unsigned char*)map_iter_prec)))-1);
					// ==> on diminue de 1 le dernier length dans l'itération précédante
				*((unsigned char*)(map_iter_prec+nb_1_successif_iter_prec)) = (unsigned char)remplir;
				unmap_map_iter_prec();
	
				num = num - LENGTH; //-LENGTH et pas -(LENGTH-1) car on a fait numm++ pour lui rajouter le byte identique présent à la fin du mapping précédant
				//faire le set_output_repeated(copi-1,num); mais attention : d'abord mettre un byte, puis le faire !
				set_one_output(copi-1);
				set_output_repeated(copi-1,num);
			}
			else{
				void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
				unsigned int remplir = (unsigned int) LENGTH;
				*((unsigned char*)map_iter_prec) = (unsigned char)remplir;
				unmap_map_iter_prec();
				num = num - LENGTH; //-LENGTH et pas -(LENGTH-1) car on a fait numm++ pour lui rajouter le byte identique présent à la fin du mapping précédant
				set_output_repeated(copi-1,num);
			}
		}
		else{
			set_output_repeated(copi-1,num);
		}
	}
	else{
		set_output_repeated(copi-1,num);
	}
}

/*
fonction qui gère l'interraction avec la fin de la zone précédemment mappée en mémoire, dans le cas d'une répétition d'un byte plus petite que LENGTH
*/
int compress_num(char courant,int num){
	if(same_byte(courant)==1){
		if(last_byte_repeated()==1){
			if((LENGTH-nb_repetition_last_byte_iter_prec)>=num){
				num = 0;
				nb_repetition_last_byte_iter_prec += num;
				void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
				unsigned int tmp = (unsigned int) nb_repetition_last_byte_iter_prec;
				*((unsigned char*)map_iter_prec) = (unsigned char)tmp;
				unmap_map_iter_prec();
			}
			else{
				nb_repetition_last_byte_iter_prec = LENGTH;
				void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
				unsigned int tmp = (unsigned int) nb_repetition_last_byte_iter_prec;
				*((unsigned char*)map_iter_prec) = (unsigned char)tmp;
				unmap_map_iter_prec();
				num = num - (LENGTH-nb_repetition_last_byte_iter_prec);
			}
		}
		else if(last_byte_succession_1()==1){ 
			num = -1;
		}
	}
	return num;
}

void compress_case_num_smaller_length(void* copi,int num,int i){
	int copy_num = 0;
	compress_nb_1_successif(copi-num-1); 
	if((i==0)&&(iteration!=0)){ 
		copy_num = num;
		num = compress_num(*((char*)(copi-1)),num);
		if(num<0){ //le dernier byte de la phase précédente n'est répété qu'une fois et a son length qui le précède
			num = copy_num;
			num++;
			int tmp = nb_1_successif_iter_prec%(LENGTH-1);
			if((tmp>1)||(tmp==0)){ //un length ne précède pas le dernier byte du mapping précédent
				void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
				if(num>LENGTH){
					unsigned int remplir = (unsigned int) LENGTH;
					*((unsigned char*)map_iter_prec) = (unsigned char)(((unsigned int)(*((unsigned char*)map_iter_prec)))-1);
						// ==> on diminue de 1 le dernier length dans l'itération précédante
					*((unsigned char*)(map_iter_prec+nb_1_successif_iter_prec)) = (unsigned char)remplir;
					unmap_map_iter_prec();
					num = num-LENGTH; //-LENGTH et pas -(LENGTH-1) car on a fait numm++ pour lui rajouter le byte identique présent à la fin du mapping précédant
					//faire le set_output_repeated(copi-1,num); mais attention : d'abord mettre un byte, puis le faire !
					set_one_output(copi-1);
					set_output_repeated(copi-1,num);
				}
				else{
					unsigned int remplir = (unsigned int) num;
					*((unsigned char*)map_iter_prec) = (unsigned char)(((unsigned int)(*((unsigned char*)map_iter_prec)))-1);
						// ==> on diminue de 1 le dernier length dans l'itération précédante
					*((unsigned char*)(map_iter_prec+nb_1_successif_iter_prec)) = (unsigned char)remplir;
					unmap_map_iter_prec();
				}
			}
			else if(tmp==1){ //il y a un length qui précède le dernier byte de la dernière section
				void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
				if(num > LENGTH){
					unsigned int remplir = (unsigned int) LENGTH;
					*((unsigned char*)map_iter_prec) = (unsigned char)(remplir);
					unmap_map_iter_prec();
					num = num-LENGTH;
					set_output_repeated(copi-1,num);
				}
				else{
					unsigned int remplir = (unsigned int) num;
					*((unsigned char*)map_iter_prec) = (unsigned char)(remplir);
					unmap_map_iter_prec();
				}
			}
		}
		else if(num!=0){
			set_output_repeated(copi-1,num);
		}
	}
	else {
		set_output_repeated(copi-1,num);
	}
}


void compress_case_num_1_length(void* copi,int num,int i){
	if((i==0)&&(iteration!=0)){ 
		char courant = *((char*)(copi-1));
		if(same_byte(courant)==1){ 
			if(last_byte_repeated()==1){
				if(nb_repetition_last_byte_iter_prec == LENGTH){
					set_output_repeated(copi-1,num);
				}
				else{
					void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
					unsigned int tmp = (unsigned int) (nb_repetition_last_byte_iter_prec+1);
					*((unsigned char*)map_iter_prec) = (unsigned char)tmp;
					unmap_map_iter_prec();
				}
			}
			else if(last_byte_succession_1()){
				int tmp = nb_1_successif_iter_prec%(LENGTH-1);
				void* map_iter_prec = give_ptr_debut_map_prec(); //on modifie la fin du mapping fait à la fin de l'itération précédente
				if((tmp>1)||(tmp==0)){
					unsigned int remplir = (unsigned int) 2;
					*((unsigned char*)map_iter_prec) = (unsigned char)(((unsigned int)(*((unsigned char*)map_iter_prec)))-1);
						// ==> on diminue de 1 le dernier length dans l'itération précédante
					*((unsigned char*)(map_iter_prec+nb_1_successif_iter_prec)) = (unsigned char)remplir;
					unmap_map_iter_prec();
					set_one_output(copi-1); //on met le byte qui se répète entre les deux coupures
				}
				else{
					unsigned int remplir = 2;
					*((unsigned char*)map_iter_prec) = (unsigned char)remplir;
					unmap_map_iter_prec();
				}
			}
		}
		else{ 
			if(last_byte_repeated()==1){
				nb_1_successif++;
			}
			else if(last_byte_succession_1()){
				nb_1_successif++;
				check_length_last_byte = 1; //forcera un traitement spécial dans compress_nb_1_successif
			}
		}
	}
	else{
		nb_1_successif++;
	}
}

/*
fonction de compression d'une partie de fichier mappée en mémoire
*/
void compress(void *src_input,int len){ 
	void *copi = src_input;
	int num = 0;
	int i = 0;
	while(copi<=(src_input+len-1)){
		num = use_give_nb_same_byte(&copi);
		if(num>LENGTH){
			compress_case_num_bigger_length(copi,num,i);
		}
		else if(num!=1){
			compress_case_num_smaller_length(copi,num,i); 
		}
		else if(num==1){
			compress_case_num_1_length(copi,num,i);
		}
		if(copi == src_input+len){ //cas de la dernière itération dans la zone couramment mappée en mémoire (on rajoute une adresse par rapport à la condition dans le while car le copi a été avancé par use_give_nb_same_byte)
			if(num==1){
				offset_last_length_iter_prec = taille_output_tot-1-nb_1_successif; //offset du byte dans l'output qui correspond au dernier byte "length" de l'itération courante
			}
			else{
				offset_last_length_iter_prec = taille_output_tot -2;
			}
			compress_nb_1_successif(copi-1);
		}
		i++;
	}
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%% Fonction du traitement du mapping en plusieurs étapes %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


/*
calcul du bon offset de l'output et de la taille de l'output à mapper (taille_to_map_output)
*/
void adjust_offset_output(){
	int tmp = taille_output_tot-taille_out;
	if(tmp==0){
		offset_output = 0;
		taille_to_map_output = taille_out;
		offset_in_mapped_output = 0;
	}
	else {
		int taille_page = getpagesize();
		offset_output = ((tmp)/(taille_page))*taille_page;
		taille_to_map_output = taille_out+(tmp%(taille_page));
		offset_in_mapped_output = tmp%taille_page;
	}
}

/*
traitement du mapping d'une partie de l'input
*/
void treatement(size_t length_input){

	// %%%%%%%%%%%%%%%% Mapping en mémoire du premier fichier %%%%%%%%%%%%%%%%%%

	src_input = mmap(NULL,length_input,PROT_READ,MAP_SHARED,fd_in,offset_input);
	if(src_input==NULL){
		close_all_file();
		error(-1,"mmap sur l'input");
	}

	// %%%%%%%%%%%%%%% calcul de la taille de l'output %%%%%%%%%%%%%%%%%%%

	count_length_output(src_input,(int)length_input);

	taille_output_tot += taille_out; //mis à jour de la taille totale de l'output
	nb_1_successif = 0;

	adjust_offset_output(); //ajuste l'offset de l'output


	// %%%%%%%%%%%%%%%% Mapping en mémoire du second fichier %%%%%%%%%%%%%%%%%%

	int err = lseek(fd_out,taille_output_tot-1, SEEK_SET);//-1 au deuxième argument car on commence à l'indice 0
	if (err == -1) {
		close_all_file();
  		error(err,"lseek sur output");
	}
	
	char dummy=0;
	err = write (fd_out, &dummy, sizeof(char));
	if (err != 1) {
		close_all_file();
   		error(err,"write du dummy sur output");
 	}

	
	src_output = mmap(NULL,taille_to_map_output,PROT_WRITE|PROT_READ,MAP_SHARED,fd_out,offset_output);
	if(src_output==NULL){
		close_all_file();
		error(-1,"mmap sur l'output");
	}
	current_mmap_output = (unsigned char*)(src_output + offset_in_mapped_output);

	// %%%%%%%%%%%%%%%%% Compression %%%%%%%%%%%%%%%%
	
	compress(src_input,(int)length_input);
	nb_1_successif = 0;
	
	// %%%%%%%%%%%%%%%%% Ecriture sur le disque (non garrantie par munmap) %%%%%%%%%%%%%%

	err = msync(src_output,(size_t)taille_to_map_output,MS_SYNC);
	if(err!=0){
		close_all_file();
		error(err,"msync");
	}

	// %%%%%%%%%%%%%%%%% Unmapping des fichiers %%%%%%%%%%%%%%%%%%%%

	err = munmap(src_input,length_input);
	if(err<0) {
		close_all_file();
		error(err,"munmap sur l'input");
	}

	err = munmap(src_output,taille_to_map_output);
	if(err<0) {
		close_all_file();
		error(err,"munmap sur l'output");
	}


}

/*
la fonction traite l'encode en mappant l'input en plusieurs fois
*/
void make_encode(){
	offset_input = 0;
	offset_output = 0;
	int tmp = 0;
	
	// %%%%%%%%%%%%%%% Ouverture des fichiers %%%%%%%%%%%%%%%

	open_file();
	// %%%%%%%%%%%%%% Traitement du fichier mappé en plusieurs fois %%%%%%%%%%%%%%%

	while((tmp=(int)input_stat.st_size-(int)offset_input)>0){
		taille_out = 0;
		if(tmp>taille_max){
			treatement((size_t)taille_max);
		}
		else {
			treatement((size_t)tmp);
		}
		iteration++;
		offset_input += (off_t)min(tmp,taille_max);
	}

	// %%%%%%%%%%%%%%%% Fermeture des fichiers %%%%%%%%%%%%%%%%%%%%%
	close_all_file();
}


int main(int argc, char *argv[])
{

	taille_max = 2*getpagesize(); 
	
	// %%%%%%%%%%%%%%% Vérification du bon nombre d'arguments %%%%%%%%%%%%

	if(argc!=3){
		fprintf(stderr,"Erreur à l'appel : il faut deux arguments (le nom du fichier à compresser suivi de celui du fichier compressé)\n");
		return EXIT_FAILURE;
	}

	// %%%%%%%%%%%%%%%%% Récupération des chemins d'accès %%%%%%%%%%%%%%%%

	input = argv[1];
	output = argv[2];

	// Encodage

	make_encode();	
	
	// %%%%%%%%%%%%%%%% Libération des ressources utilisées %%%%%%%%%%%%%%


	return EXIT_SUCCESS;
}
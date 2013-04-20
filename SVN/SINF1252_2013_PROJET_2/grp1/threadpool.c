#include "filterBand.h"

//constante à initialiser en début de programme
static int NFILTERS;
static int NPACK; //nombre de paquet de ligne : toutes les images comprises
static int NBLUR; //nombre de filtre blur
static int NIMAGE;

static elem_buf*** mat_buf; //la matrice dont les lignes sont les buffer. La première ligne de cette matrice sera remplie par la méthode qui charge et divise les images.
static int** mat_blur; //la matrice pour gérer les blur
static pthread_mutex_t** blur_mutex; //chaque élément du tableau doit être protégé par un mutex. Car, pour un blur, on peut incrémenter deux cases en même temps, mais pas en même temps la même case.
static sem_t* can_copi;//matrice de sémaphores pour gérer les copies des images
static int** choosen_c; //pour chaque blur, une ligne avec autant d'éléments que d'images. L'élément vaut 1 si l'image a déjà été copiée, 0 sinon.
static pthread_mutex_t** choosen_c_mutex;
static int** copi_is_done;
static pthread_mutex_t** copi_is_done_mutex;
static int** can_i_take_c;
static pthread_mutex_t* can_i_take_c_mutex;
static sem_t* full; //nos sémaphores pour le producteur-consommateur
static sem_t* empty;
static pthread_mutex_t* buf_mutex; //il faut protéger les buffers, comme expliqué dans les notes du producteur-consommateur.
static int* f_isblur;
static int* f_beforeblur;
static int* to_pass;// pour emêcher le passage dans la boucle du producteur s'il y a déjà eu NFILTERS passage.
static pthread_mutex_t* to_pass_mutex;

void initializeStatic(int nFilters, int nPack, int nBlur, int nImage) {
	NFILTERS = nFilters+1;
	NPACK = nPack+1;
	NBLUR = nBlur+1;
	NIMAGE = nImage+1;
	
	mat_buf = (elem_buf***)malloc(sizeof(elem_buf**)*NFILTERS);
	if (mat_buf == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	int i;
	for (i=0; i<NFILTERS; i++) {
		mat_buf[i] = (elem_buf**)malloc(sizeof(elem_buf*)*NPACK);
		if (mat_buf[i] == NULL)
			error(-1,"Erreur de malloc dans initializeStatic\n");
		int j;		
		for (j=0; j<NPACK; j++) {
			mat_buf[i][j]=NULL;
		} 
	}
	
	mat_blur = (int**)malloc(sizeof(int*)*NBLUR);
	if (mat_blur == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");

	can_copi = (sem_t*)malloc(sizeof(sem_t)*NBLUR);
	if (can_copi == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");

	blur_mutex = (pthread_mutex_t**)malloc(sizeof(pthread_mutex_t*)*NBLUR);
	if (blur_mutex == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	choosen_c = (int**)malloc(sizeof(int*)*NBLUR);
	if (choosen_c == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");

	choosen_c_mutex = (pthread_mutex_t**)malloc(sizeof(pthread_mutex_t*)*NBLUR);
	if (choosen_c_mutex == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	copi_is_done = (int**)malloc(sizeof(int*)*NBLUR);
	if (copi_is_done == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	copi_is_done_mutex = (pthread_mutex_t**)malloc(sizeof(pthread_mutex_t*)*NBLUR);
	if (copi_is_done_mutex == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	can_i_take_c = (int**)malloc(sizeof(int*)*NBLUR);
	if (can_i_take_c == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	can_i_take_c_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*NBLUR);
	if (can_i_take_c_mutex == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	for (i=0; i<NBLUR; i++) {
		
		mat_blur[i] = (int*)calloc(NIMAGE,sizeof(int));
		if (mat_blur[i] == NULL)
			error(-1,"Erreur de calloc dans initializeStatic\n");

		blur_mutex[i] = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*NIMAGE);
		if (blur_mutex == NULL)
			error(-1,"Erreur de malloc dans initializeStatic\n");
		
		choosen_c[i] = (int*)calloc(NIMAGE,sizeof(int));
		if (choosen_c[i] == NULL)
			error(-1,"Erreur de calloc dans initializeStatic\n");

		choosen_c_mutex[i] = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*NIMAGE);
		if (choosen_c_mutex[i] == NULL)
			error(-1,"Erreur de calloc dans initializeStatic\n");
		
		copi_is_done[i] = (int*)calloc(NIMAGE,sizeof(int));
		if (copi_is_done[i] == NULL)
			error(-1,"Erreur de calloc dans initializeStatic\n");

		copi_is_done_mutex[i] = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*NIMAGE);
		if (copi_is_done_mutex[i] == NULL)
			error(-1,"Erreur de malloc dans initializeStatic\n");
		
		can_i_take_c[i] = (int*)calloc(NIMAGE,sizeof(int));
		if (can_i_take_c[i] == NULL)
			error(-1,"Erreur de calloc dans initializeStatic\n");
	}
	
	full = (sem_t*)malloc(sizeof(sem_t)*NFILTERS);
	if (full == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	empty = (sem_t*)malloc(sizeof(sem_t)*NFILTERS);
	if (empty == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	buf_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*NFILTERS);
	if (buf_mutex == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	f_isblur = (int*)calloc(NFILTERS,sizeof(int));
	if (f_isblur == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	f_beforeblur = (int*)calloc(NFILTERS,sizeof(int));
	if (f_beforeblur == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");

	to_pass = (int*)calloc(NFILTERS,sizeof(int));
	if (to_pass == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");
	
	to_pass_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)*NFILTERS);
	if (to_pass_mutex == NULL)
		error(-1,"Erreur de malloc dans initializeStatic\n");

	NFILTERS--;
	NPACK--;
	NBLUR--;
	NIMAGE--;
}

void freeStatic() {

	NFILTERS++;
	NPACK++;
	NBLUR++;
	NIMAGE++;
	int i;
	for (i=0; i<NFILTERS; i++) {
		if (mat_buf[i] != NULL)
			free(mat_buf[i]);
	}
	if (mat_buf != NULL)
		free(mat_buf);
	mat_buf = NULL;
	
	for (i=0; i<NBLUR; i++) {
		if (mat_blur != NULL) {
			if (mat_blur[i] != NULL)
				free(mat_blur[i]);
		}
		if (blur_mutex != NULL) {
			if (blur_mutex[i] != NULL)
				free(blur_mutex[i]);
		}
		
		if (choosen_c != NULL) {
			if (choosen_c[i] != NULL)
				free(choosen_c[i]);
		}
		if (choosen_c_mutex != NULL) {
			if (choosen_c_mutex[i] != NULL)
				free(choosen_c_mutex[i]);
		}

		if (copi_is_done != NULL) {
			if (copi_is_done[i] != NULL)
				free(copi_is_done[i]);
		}
		if (copi_is_done_mutex != NULL) {
			if (copi_is_done_mutex[i] != NULL)
				free(copi_is_done_mutex[i]);
		}
		if (can_i_take_c != NULL) {	
			if (can_i_take_c[i] != NULL)
				free(can_i_take_c[i]);
		}
	}
	
	if (mat_blur != NULL)
		free(mat_blur);
	if (can_copi != NULL)
		free(can_copi);
	if (blur_mutex != NULL)
		free(blur_mutex);
	
	if (choosen_c != NULL)
		free(choosen_c);
	if (choosen_c_mutex != NULL)
		free(choosen_c_mutex);
	if (copi_is_done != NULL)
		free(copi_is_done);
	if (copi_is_done_mutex != NULL)
		free(copi_is_done_mutex);
	if (can_i_take_c != NULL)
		free(can_i_take_c);
	if (can_i_take_c_mutex != NULL)
		free(can_i_take_c_mutex);
	
	mat_blur = NULL;
	can_copi = NULL;
	blur_mutex = NULL;
	choosen_c = NULL;
	choosen_c_mutex = NULL;
	copi_is_done = NULL;
	copi_is_done_mutex = NULL;
	can_i_take_c = NULL;
	can_i_take_c_mutex = NULL;
	
	if (full != NULL)
		free(full);
	if (empty != NULL)
		free(empty);
	if (buf_mutex != NULL)
		free(buf_mutex);
	if (f_isblur != NULL)
		free(f_isblur);
	if (f_beforeblur != NULL)
		free(f_beforeblur);
	if (to_pass != NULL)
		free(to_pass);
	if (to_pass_mutex != NULL)
		free(to_pass_mutex);
	
	full = NULL;
	empty = NULL;
	buf_mutex = NULL;
	f_isblur = NULL;
	f_beforeblur = NULL;
	to_pass = NULL;
	to_pass_mutex = NULL;
}

int lcm(int a, int b) {
	int i;
	for (i=1; i<a*b; i++) {
		if (i%a == 0 && i%b == 0) {
			return i;
		}
	}
	return i;
}

int lcmArray(int a[], int len) {
	
	int lc = lcm(a[0],a[1]);
	int i;
	if (len > 2) {
		for (i=2; i<len; i++) {
			lc = lcm(lc,a[i]);
		}
	}
	return lc;
}

int maxArray(int a[], int len) {
	int max = a[0];
	int i;
	for (i =1; i<len; i++) {
		if (max<a[i]) {
			max = a[i];
		}
	}
	return max; 
}
	
void imagesCut(struct image* image[]) {
	int nPack = NPACK/NIMAGE;
	int i;

	for (i=0; i<NIMAGE; i++) {
		
		int mod = image[i]->height%nPack;
		if (mod > 0)
			mod = 1;
		int band = (image[i]->height)/nPack + mod;
		
		int j;
		for (j=0; j<nPack; j++) {
			mat_buf[0][i*nPack+j] = (elem_buf*)malloc(sizeof(elem_buf));
			if (mat_buf[0][i*nPack+j] == NULL)
				error(-1,"Erreur de malloc dans imagesCut\n");
			mat_buf[0][i*nPack+j]->img = image[i];
			mat_buf[0][i*nPack+j]->num_img = i;
			mat_buf[0][i*nPack+j]->begin = j*band;
			mat_buf[0][i*nPack+j]->finish = (j+1)*band-1;
		}
	}
}

int initializeBlur(char* filters[]){
	int i;
	int n = 1;
	for (i=0; i<NFILTERS; i++) {
		if (!strncasecmp(filters[i],"blur",4)) {
			f_isblur[i] = n;
			if (i>0) {
				f_beforeblur[i-1] = n;
			}
			n++;
		} else {
			f_isblur[i] = 0;
			if (i>0) {
				f_beforeblur[i-1] = 0;
			}
		}
	}
	if(f_isblur[0]!=0) {
		for (i=0;i<NIMAGE; i++) {
			mat_blur[0][i] = NPACK/NIMAGE;
		}
	}
	else {
		for (i=0;i<NIMAGE; i++) {
			mat_blur[0][i] = 0;
		}
	}
	return n-1;
}

int numberBlur(char* filters[], int len) {
	int n = 0;
	int i;
	for (i=0; i<len; i++) {
		if (!strncasecmp(filters[i],"blur",4))
			n++;
	}
	return n;
}

int image_copi(int img, int num) {
	int i;
	int j = 0;
	struct image* image = NULL;
	struct pixel* pixels = NULL;

	int err = pthread_mutex_lock(&(buf_mutex[num])); //pour éviter que plusieurs threads accèdent a mat_buf pour faire deux copie différentes en même temps (sinon helgrind râle)
	if (err!=0)
		error(err,"pthread_mutex_lock pour buf_mutex dans image_copi");	

	for(i=0;i<NPACK;i++) {
		if(mat_buf[num][i]!=NULL) {
			if(mat_buf[num][i]->num_img==img) {
				if(j==0) {
					image = (struct image*)malloc(sizeof(struct image));
					if (image == NULL) 
						error(-1,"Erreur de malloc dans image_copi\n");
					image->width = mat_buf[num][i]->img->width;
					image->height = mat_buf[num][i]->img->height;
					pixels = (struct pixel*)malloc(sizeof(struct pixel)*image->width*image->height);
					if (pixels == NULL) {
						free(image);
						error(-1,"Erreur de malloc dans image_copi\n");
					}
		
					memcpy(pixels,mat_buf[num][i]->img->pixels,sizeof(struct pixel)*image->width*image->height);
	
					image->pixels = pixels;
					mat_buf[num][i]->read_only = image;//plop
					j++;
				}
				else {
					mat_buf[num][i]->read_only = image;
				}
			}
		}
	}

	err = pthread_mutex_unlock(&(buf_mutex[num]));
	if (err!=0)
		error(err,"pthread_mutex_unlock pour buf_mutex dans image_copi");	

	return 0;
}

int image_copi_destroy(int img, int num) {
	int i;
	int j = 0;
	for(i=0;i<NPACK;i++) {
		if(mat_buf[num][i]!=NULL) {
			if((mat_buf[num][i]->num_img==img)&&(mat_buf[num][i]->read_only!=NULL)) {
				if(j==0) {
					if ((mat_buf[num][i]->read_only)->pixels != NULL) {
						free((mat_buf[num][i]->read_only)->pixels);
					}
					free(mat_buf[num][i]->read_only);
					mat_buf[num][i]->read_only = NULL;
					j++;
				}
				else {
					mat_buf[num][i]->read_only = NULL;
				}
			}	
		}
	}
	return 0;
}

void free_mat_buf() {
	int i;
	int j;
	for(i=0;i<NPACK;i++) {
		for(j=0;j<NFILTERS+1;j++) {
			if(mat_buf[j][i]!=NULL) {
				free(mat_buf[j][i]);
			}
		}
	}
}

void clean_up_image_copi() {
	int i;
	int j;
	for(i=0;i<NFILTERS+1;i++) {
		for(j=0;j<NIMAGE;j++) {
			image_copi_destroy(j,i);
		}
	}
}

void error(int err, char *msg) {
	fprintf(stderr,"%s a retourné %d. Message d'erreur : %s\n",msg,err,strerror(errno));
	clean_up_image_copi();
	free_mat_buf();
	clean_malloc_file();
	freeStatic();
	cleanImage();
	exit(EXIT_FAILURE);
}

//reçoit une ligne de mat_buffer et renvoit le premier pointeur non-NULL qu'il croise et met NULL à sa place dans la ligne de mat_buf. Erreur si tous les pointeurs sont NULL.
elem_buf *take_elem(int ligne_mat_buf) {
	int i=0;
	while(i<NPACK) {
		if(mat_buf[ligne_mat_buf][i]==NULL) {
			i++;
		} else {
			elem_buf *retour = mat_buf[ligne_mat_buf][i]; 
			mat_buf[ligne_mat_buf][i] = NULL;
			return retour;
		}
	}
	error(-1,"take-elem n'a pas trouvé d'éléments non-nul et ");
	return NULL;
}

/*
 * même principe que take_elem mais cette fois-ci, on permet de spécifier l'image qu'on veut que l'elem_buf retourné contienne. Utilisé par le productor dans le cas du filtre blur.
 */
elem_buf *take_elem_image(int ligne_mat_buf,int nimage) {
	int i=0;
	while(i<NPACK) {
		if((mat_buf[ligne_mat_buf][i]==NULL)||(((mat_buf[ligne_mat_buf][i])->num_img)!=nimage)) {
			i++;
		} else {
			elem_buf *retour = mat_buf[ligne_mat_buf][i]; 
			mat_buf[ligne_mat_buf][i] = NULL;
			return retour;
		}
	}
	error(-1,"take-elem_image n'a pas trouvé d'éléments non-nul ayant l'image spécifié et ");
	return NULL;
}

//utilisé par le producteur. On lui donne juste le numéro du filtre et il prend un élément du buffer précédent le filtre et le renvoit. Aussi, il faut lui donner le numéro de l'image. S'il vaut -1, c'est que toutes les images sont bonnes.
elem_buf *consommator(int num_filter, int nimage) {
	int err;
	err = sem_wait(&(full[num_filter]));//le numéro du filtre désigne la ligne de mat_buf où il faut lire : si on a le filtre 0, la ligne 0 est remplie donc il faut lire dedans
	if (err!=0) 
		error(err,"sem_wait pour full dans consommator");
	
	err = pthread_mutex_lock(&(buf_mutex[num_filter]));
	
	if (err!=0)
		error(err,"pthread_mutex_lock pour buf_mutex dans consommateur");
	
	//il faut prendre un pointeur elem_buffer de la ligne précédente, le renvoyer et mettre cet élément à null dans le buffer précédent : via take-elem
	elem_buf *retour;
	if(nimage==-1) {
		retour = take_elem(num_filter);
		
	} else {
		retour = take_elem_image(num_filter,nimage);
		
	}
	
	err = pthread_mutex_unlock(&(buf_mutex[num_filter]));
	if (err!=0)
		error(err,"pthread_mutex_unlock pour buf_mutex dans consommateur");
	
	err = sem_post(&(empty[num_filter]));
	if (err!=0)
		error(err,"sem_post pour empty dans consommator");

	return retour;
}

//insère un double pointeur ver-f blur -t 2 /home/cyril/Desktop/All_my_work/images_projet_2/images /home/cyril/Desktop/All_my_work/images_projet_2/Filtered_imagess un elem_buf dans une ligne une ligne de mat_buf et le met à une case non-null de la ligne. Si toutes les case sont null, renvoit un erreur.
void insert_elem(elem_buf **paquet, int lmat_buf) {
	int i=0;
	
	while(i<NPACK) {
		if((mat_buf[lmat_buf][i])!=NULL) {
			i++;
		} else {
			mat_buf[lmat_buf][i]=*paquet;
			i=NPACK+1;
		}
	}
	if(i==NPACK)
		error(-1,"insert_elem n'a pas trouvé de place vide et");
}

int check_lmat_blur(int L) {

	int i=0;
	int c = -1;
	int err;
	int nb=0;
	while(i<NIMAGE) {
		nb = i;
		err = pthread_mutex_lock(&(blur_mutex[L][i]));
		if (err!=0)
			error(err,"pthread_mutex_lock pour blur_mutex dans check_lmat_blur");
		if(((mat_blur[L][i])==NPACK/NIMAGE)&&(can_i_take_c[L][i]<NPACK/NIMAGE)) {		
			c=i;
			can_i_take_c[L][i]++;
			nb=i;
			i=NIMAGE;
		}
		err = pthread_mutex_unlock(&(blur_mutex[L][nb]));
		if (err!=0)
			error(err,"pthread_mutex_unlock pour blur_mutex dans check_lmat_blur");
		i++;
	}
	return c;
}

void *productor(void *params) {
	prod_param *parg = (prod_param *) params;
	int err;
	
	//3 cas : on est un blur, on est avant un blur ou bien le cas normal. Si on est avant un blur, il faut lancer la copie de l'image quand, dans mat_blur, on a, pour l'image, le nombre de paquet
	//en lesquels elle a été divisée. Pour cela, il faut un tableau global npackim[NIMAGE] qui donne, pr chaque image, en combien de paquet elle a été divisée ou bien, on divise chaque image en
	//NPACK/NIMAGE de paquet (on divise chacune des images avec le ppcm du nombre de thread), ce qui fait qu'on a le nombre de paquet dans une image avec le nombre de paquet total
	//ces 3 cas sont dans le while
	while(1) {

		err = pthread_mutex_lock(&(to_pass_mutex[parg->num_filter]));
		if (err!=0)
			error(err,"pthread_mutex_lock pour to_pass_mutex dans producteur");
		if(to_pass[parg->num_filter]>=NPACK){
			err = pthread_mutex_unlock(&(to_pass_mutex[parg->num_filter]));
			if (err!=0)
				error(err,"pthread_mutex_unlock pour to_pass_mutex dans producteur");
			pthread_exit(NULL);
		}
		to_pass[parg->num_filter]++;
		err = pthread_mutex_unlock(&(to_pass_mutex[parg->num_filter]));
		if (err!=0)
			error(err,"pthread_mutex_unlock pour to_pass_mutex dans producteur");

		printf(".");
		fflush(stdout);
		//cas du filtre blur
		if((parg->isblur)!=0) {
			
			//1e étape : vérifer qu'il y a un des éléments de matblur en la ligne isblur-1
			//printf("Je suis dans le cas blur\n");
			//err = pthread_mutex_lock(&(check_for_c_mutex[(parg->isblur)-1])); //deux threads ne peuvent rechercher un c en même temps sinon ils copieront la même image

			err = sem_wait(&(can_copi[(parg->isblur)-1])); 
			if (err!=0)
				error(err,"sem_wait sur can_copi dans productor");

			err = pthread_mutex_lock(&(can_i_take_c_mutex[(parg->isblur)-1]));//ce lock est nécessaire car la sémaphore ne fait des wait que pour empêcher des thread d'entrer quand il n'y a pas d'image prête pour la copie. Mais la sémaphore  peut être incrémentée par le fait que deux images différentes peuvent être prête presque en même temps pour la copie. Alors, il faut faire un lock sur le can_i_take_c_mutex pour éviter qu'il n'y ait deux threads qui entrent en même temps dans le check_lmat_blur, ce qui poserait des problème vu qu'une des threads pourraient lire dans can_i_take_c pendant que l'autre écrit dedans.
			if (err!=0)
				error(err,"pthread_mutex_lock pour can_i_take_c_mutex dans producteur du cas blur");

			int c = check_lmat_blur((parg->isblur)-1);
			if(c==-1) {
				error(-1,"check_lmat_blur a renvoyé -1");
			}
			
			if (can_i_take_c[(parg->isblur)-1][c] < NPACK/NIMAGE) {
				err = sem_post(&(can_copi[(parg->isblur)-1])); 
				if (err!=0)
					error(err,"sem_post sur can_copi dans productor");
			}
			
			err = pthread_mutex_unlock(&(can_i_take_c_mutex[(parg->isblur)-1]));
			if (err!=0)
				error(err,"pthread_mutex_unlock pour can_i_take_c_mutex dans producteur du cas blur");

			//	error(err,"pthread_mutex_unlock pour check_for_c_mutex dans producteur du cas blur");

			//2e étape : on a une image dont le précédent a été appliquer sur toutes les parties. On la copie

			err = pthread_mutex_lock(&(copi_is_done_mutex[(parg->isblur)-1][c]));
			if (err!=0)
				error(err,"pthread_mutex_lock pour copi_is_done_mutex dans producteur du cas blur pour image_copi");

			if(copi_is_done[(parg->isblur)-1][c]==0) {

				err = image_copi(c,parg->num_filter); //on donne le numéro correspondant à l'image et le numéro de la ligne dans le mat_buf où les parties de cette image se trouve. Il faut mettre à jour les éléments read-only des pointeur vers des elem_buf dans cette ligne (note, il est nécessaire d'avoir une structure copie par image mais pas plus ! Car quand on fait une 2e copie pour un 2e blur, alors on peut écraser l'ancienne copie !)
				if (err!=0) {
					error(err,"Erreur de image_copi");
				}
				copi_is_done[(parg->isblur)-1][c]++;				
			}
			err = pthread_mutex_unlock(&(copi_is_done_mutex[(parg->isblur)-1][c]));
			if (err!=0)
				error(err,"pthread_mutex_unlock pour copi_is_done_mutex dans producteur du cas blur pour image_copi");

			//3e étape : on a fait les copies. Mnt, on doit appeler le consommateur en lui-disant de consommer un elem-buffer mais dont l'image est c
			elem_buf *paquet = consommator(parg->num_filter,c);//mettre c en 2e argument permet de dire que tous les elem_buf sont bons, quelque soit l'image
			apply_filter(paquet,parg->num_filter);
			
			err = sem_wait(&(empty[(parg->num_filter)+1])); //amélioration possible du code : ce sem_wait ne sert pas vraiment, vu que on sait de toute façon que les buffer ne vont pas être rempli
			if (err!=0)
				error(err,"sem_wait sur empty dans productor");
			
			err = pthread_mutex_lock(&(buf_mutex[(parg->num_filter)+1]));
			if (err!=0)
				error(err,"pthread_mutex_lock pour buf_mutex dans producteur");
			
			insert_elem(&paquet,(parg->num_filter)+1);
			
			err = pthread_mutex_lock(&(choosen_c_mutex[(parg->isblur)-1][c]));
			if (err!=0)
				error(err,"pthread_mutex_lock pour choosen_c_mutex dans producteur, check_lmat_blur");

			choosen_c[(parg->isblur)-1][c]++;

			if((choosen_c[(parg->isblur)-1][c]==NPACK/NIMAGE)) {
				err = image_copi_destroy(c,parg->num_filter+1);

				if (err!=0)
					error(err,"Erreur de image_copi_destroy");
			}

			err = pthread_mutex_unlock(&(choosen_c_mutex[(parg->isblur)-1][c]));
			if (err!=0)
				error(err,"pthread_mutex_unlock pour choosen_c_mutex dans producteur");
			
			err = pthread_mutex_unlock(&(buf_mutex[(parg->num_filter)+1]));
			if (err!=0)
				error(err,"pthread_mutex_unlock pour buf_mutex dans producteur");
			
			err = sem_post(&(full[(parg->num_filter)+1])); //amélioration possible du code : ce sem_wait ne sert pas vraiment, vu que on sait de toute façon que les buffer ne vont pas être rempli
			if (err!=0)
				error(err,"sem_post sur full dans productor");
			
			//on est avant un filtre blur
			if((parg->beforeblur)!=0) {//parg_beforeblur contient 1 si c'est le premier blur, 2 si c'est le deuxième, ...

				err = pthread_mutex_lock(&(blur_mutex[(parg->beforeblur)-1][(paquet)->num_img]));
				if (err!=0)
					error(err,"pthread_mutex_lock pour blur_mutex dans producteur");
				
				(mat_blur[(parg->beforeblur)-1][(paquet)->num_img])++;

				if ((mat_blur[(parg->beforeblur)-1][(paquet)->num_img])==NPACK/NIMAGE) {
					err = sem_post(&(can_copi[(parg->beforeblur)-1])); 
					if (err!=0)
						error(err,"sem_post sur can_copi dans productor, before_blur");
				}
				
				err = pthread_mutex_unlock(&(blur_mutex[(parg->beforeblur)-1][(paquet)->num_img]));
				if (err!=0)
					error(err,"pthread_mutex_unlock pour blur_mutex dans producteur");
			}
			
		} else {
		//cas normal

			elem_buf *paquet = consommator(parg->num_filter,-1);//mettre -1 en 2e argument permet de dire que tous les elem_buf sont bons, quelque soit l'image
			apply_filter(paquet,parg->num_filter);
			err = sem_wait(&(empty[(parg->num_filter)+1])); //amélioration possible du code : ce sem_wait ne sert pas vraiment, vu que on sait de toute façon que les buffer ne vont pas être rempli
			if (err!=0)
				error(err,"sem_wait sur empty dans productor");
			err = pthread_mutex_lock(&(buf_mutex[(parg->num_filter)+1]));
			if (err!=0)
				error(err,"pthread_mutex_lock pour buf_mutex dans producteur");
			insert_elem(&paquet,(parg->num_filter)+1);
			err = pthread_mutex_unlock(&(buf_mutex[(parg->num_filter)+1]));
			if (err!=0)
				error(err,"pthread_mutex_unlock pour buf_mutex dans producteur");
			
			err = sem_post(&(full[(parg->num_filter)+1])); //amélioration possible du code : ce sem_wait ne sert pas vraiment, vu que on sait de toute façon que les buffer ne vont pas être rempli
			if (err!=0)
				error(err,"sem_post sur full dans productor");
			
			//on est avant un filtre blur
			if((parg->beforeblur)!=0) {//parg_beforeblur contient 1 si c'est le premier blur, 2 si c'est le deuxième, ...

				err = pthread_mutex_lock(&(blur_mutex[(parg->beforeblur)-1][(paquet)->num_img]));
				if (err!=0)
					error(err,"pthread_mutex_lock pour blur_mutex dans producteur");
				
				(mat_blur[(parg->beforeblur)-1][(paquet)->num_img])++;

				if ((mat_blur[(parg->beforeblur)-1][(paquet)->num_img])==NPACK/NIMAGE) {
					err = sem_post(&(can_copi[(parg->beforeblur)-1])); 
					if (err!=0)
						error(err,"sem_post sur can_copi dans productor, before_blur");
				}

				err = pthread_mutex_unlock(&(blur_mutex[(parg->beforeblur)-1][(paquet)->num_img]));
				if (err!=0)
					error(err,"pthread_mutex_unlock pour blur_mutex dans producteur");
			}
		}
	}
}

/*
 * fonction qui lance les threads
 * pre : reçoit un tableau avec le nombre de threads par filtre et le nombre de thread maximal
 *       l'initialisation des constantes doit être effectuées
 *       appel applyfilter via en donnant simplement des numéros comme filtre.
 *       la premiere ligne du buffer mat_buf doit être remplie
 * post : lance les threads et traite les images.
 * Attention : la fonction doit faire des join sur la dernière salve de thread.
 */
void* image_treatement(void* arrayThreads) {
	
	int* nthreads = (int*)arrayThreads;
	int nthreadmax = nthreads[0];
	int i;
	for (i=1; i<NFILTERS; i++) {
		if (nthreads[i]>nthreadmax)
			nthreadmax = nthreads[i];
	}
	
	int j;
	prod_param parg[NFILTERS][nthreadmax];
	pthread_t threads[NFILTERS][nthreadmax];
	int err;
	
	int val_init_can_copi = 0;
	
	//initialisation de la sémaphore can_copi et des mutex de blur_mutex, choosen_c_mutex, can_i_take_c_mutex, copi_is_done_mutex et de ceux de check_for_c_mutex :
	for(i=0;i<NBLUR;i++) {

		err=pthread_mutex_init(&(can_i_take_c_mutex[i]),NULL);
		if (err!=0)
			error(err,"pthread_mutex_init pour can_i_take_c_mutex");

		if ((i==0)&&(f_isblur[0]!=0)) {
			val_init_can_copi = NIMAGE;
		}
		else {
			val_init_can_copi = 0;
		}

		err = sem_init(&(can_copi[i]),0,val_init_can_copi);
		if (err!=0)
			error(err,"sem_init pour val_init_can_copi");

		for(j=0;j<NIMAGE;j++) {
			choosen_c[i][j]=0;
			copi_is_done[i][j]=0;
			can_i_take_c[i][j]=0;
			
			err=pthread_mutex_init(&(choosen_c_mutex[i][j]),NULL);
			if (err!=0)
				error(err,"pthread_mutex_init pour choosen_c_mutex");
			
			err=pthread_mutex_init(&(blur_mutex[i][j]),NULL);
			if (err!=0)
				error(err,"pthread_mutex_init pour blur_mutex");

			err=pthread_mutex_init(&(copi_is_done_mutex[i][j]),NULL);
			if (err!=0)
				error(err,"pthread_mutex_init pour copi_is_done_mutex");
		}
	}
	
	//initialisation des sémaphores full et empty et des mutex buf_mutex
	int val_init_full; //valeur initiale des sémaphores dans full
	int val_init_empty; //valeur initiale des sémaphores dans empty
	
	//initilisation des sémaphores full et empty et des mutex de isdone, de to_pass_mutex et de buf_mutex
	for(i=0;i<NFILTERS+1;i++) {
		if(i==0) {
			val_init_full = NPACK;
			val_init_empty = 0;
		} else {
			val_init_full = 0;
			val_init_empty = NPACK;
		}
		
		err = sem_init(&(full[i]),0,val_init_full);
		if (err!=0)
			error(err,"sem_init pour full");
		
		err = sem_init(&(empty[i]),0,val_init_empty);
		if (err!=0)
			error(err,"sem_init pour empty");
		
		err=pthread_mutex_init(&(buf_mutex[i]),NULL);
		if (err!=0)
			error(err,"pthread_mutex_init pour buf_mutex");
		
		err=pthread_mutex_init(&(to_pass_mutex[i]),NULL);
		if (err!=0)
			error(err,"pthread_mutex_init pour to_pass_mutex");
	}
	
	//lancement des threads des filtres
	for(i=0;i<NFILTERS;i++) {
		for(j=0;j<nthreads[i];j++) {
			(parg[i][j]).num_filter = i;
			(parg[i][j]).isblur = f_isblur[i];
			(parg[i][j]).beforeblur = f_beforeblur[i];
			err = pthread_create(&(threads[i][j]),NULL,&(productor),(void *)(&(parg[i][j])));
			if (err!=0)
				error(err,"pthread_create");
		}
	}
	
	//Attente des threads : IL FAUT LES ATTENDRE TOUTES DANS L'ORDRE !
	for(j=0;j<NFILTERS;j++) {
		for(i=0;i<nthreads[j];i++) {
			err = pthread_join(threads[j][i],NULL);
			if (err!=0)
				error(err,"pthread_join");
		}
	}

	//libération de la sémaphore can_copi et des mutex de blur_mutex, choosen_c_mutex, can_i_take_c_mutex, copi_is_done_mutex et de check_for_c_mutex
	for(i=0;i<NBLUR;i++) {

		err=pthread_mutex_destroy(&(can_i_take_c_mutex[i]));
		if (err!=0)
			error(err,"pthread_mutex_destroy pour can_i_take_c_mutex");	

		err = sem_destroy(&(can_copi[i]));
		if (err!=0)
			error(err,"sem_destroy pour can_copi");	

		for(j=0;j<NIMAGE;j++) {
			
			err=pthread_mutex_destroy(&(choosen_c_mutex[i][j]));
			if (err!=0)
				error(err,"pthread_mutex_destroy pour choosen_c_mutex");
			
			err=pthread_mutex_destroy(&(blur_mutex[i][j]));
			if (err!=0)
				error(err,"pthread_mutex_destroy pour blur_mutex");

			err=pthread_mutex_destroy(&(copi_is_done_mutex[i][j]));
			if (err!=0)
				error(err,"pthread_mutex_destroy pour copi_is_done_mutex");
		}
	}

	//libération des sémaphores full et empty, des mutex de buf_mutex, de to_pass_mutex et des mutex de isdone
	for(i=0;i<NFILTERS+1;i++) {
		err = sem_destroy(&(full[i]));
		if (err!=0)
			error(err,"sem_destroy pour full");
		
		err = sem_destroy(&(empty[i]));
		if (err!=0)
			error(err,"sem_destroy pour empty");
		
		err=pthread_mutex_destroy(&(buf_mutex[i]));
		if (err!=0)
			error(err,"pthread_mutex_destroy pour buf_mutex");

		err=pthread_mutex_destroy(&(to_pass_mutex[i]));
		if (err!=0)
			error(err,"pthread_mutex_destroy pour to_pass_mutex");
	}
	pthread_exit(NULL);
}

void* increment(void* arrayThreads) {
	
	int* nthreads = (int*)arrayThreads;

	struct timespec tim;
   	tim.tv_sec = 0;
   	tim.tv_nsec = 600*1000000;
	
	printf("\n"ANSI_COLOR_RED"Images treatment"ANSI_COLOR_RESET);
	while (nthreads[0] != 0 ) {
		printf(ANSI_COLOR_GREEN".");
		fflush(stdout);
		nanosleep(&tim,NULL);
	}
	printf(".\n\n"ANSI_COLOR_RESET);
	return NULL;
}

/////////////////////////
//     STRUCTURES      //
/////////////////////////

/*
 * définition de la structure que contiendra chaque case des buffer.
 */
typedef struct inbuf {
	struct image* img; //pointeur vers l'image
	int num_img; // numéro de l'image (il faut un numéro pour mettre correctement à jour le tableau de int)
	struct image* read_only; //image pour la copie avant le filtre blur. Attention, il faut que la copie se fasse lorsque le filtre précédents blur soit terminé sur toutes les images
	int begin; //n° de la ligne de début, où il faut faire les changements
	int finish; //n° de la ligne de fin
} elem_buf;

typedef struct argprod{
	int num_filter; //numéro du filtre à appliquer : le 1e filtre a le numéro 0 !
	int isblur; //vaut 1 si le filtre à appliquer est un blur, 0 sinon
	int beforeblur; //vaut 0 si le filtre à appliquer n'est pas un blur, 1 s'il précède le 1e blur, 2 s'il précède le 2e blur, ...
} prod_param;

/////////////////////////
//     FUNCTIONS       //
/////////////////////////

int lcm(int a, int b);
int lcmArray(int a[], int len);
int maxArray(int a[], int len);
int numberBlur(char* filters[], int len);
void initializeStatic(int nFilters, int nPack, int nBlur, int nImage);
void freeStatic();
void intializeMatrix();
void free_mat_buf();
void clean_malloc_file();
int initializeBlur(char* filters[]);
void imagesCut(struct image* img[]);
int image_copi(int img, int num);
int image_copi_destroy(int img, int num);
void error(int err, char *msg);
elem_buf *take_elem(int ligne_mat_buf);
elem_buf *take_elem_image(int ligne_mat_buf,int nimage);
elem_buf *consommator(int num_filter, int nimage);
void insert_elem(elem_buf **paquet, int lmat_buf);
void update_isdone(int nfiltre);
int check_lmat_blur(int L);
void *productor(void *params);
void* image_treatement(void* arrayThreads);
void* increment(void* arrayThreads);
void cleanImage();
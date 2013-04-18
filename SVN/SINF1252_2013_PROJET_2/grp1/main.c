#include "filterBand.h"

#define FILTERS_NUM 256
#define FILES_ARRAY_LEN 1024
#define CAT_LEN 128
#define STRING_LEN 512
#define BLOCK 20

char* files[FILES_ARRAY_LEN];
char* save[FILES_ARRAY_LEN];
struct image* images[BLOCK];

typedef struct {
	int n;
	struct image** img;
} openArgs;

int parseFilters(char str[], char* array[]);
int parseThreads(char str[], int array[]);
void treatBlock(int threadsArray[],int threadsLen, char* filtersArray[],int filtersLen, int nImage, int n);
void* openImage(void* args);
void* saveImage(void* args);

void clean_malloc_file() {
	int j;
	for (j=0; j<FILES_ARRAY_LEN; j++) {
		if (files[j]!=NULL)
			free(files[j]);
		if (save[j]!=NULL)
			free(save[j]);
	}
}

int main(int argc, char *argv[])
{
	
	////////////////////////////
	//     ARGS TREATMENT     //
	////////////////////////////
	
	// initializing needed variables
	char c;
   	char *filters = NULL;
	char *threads = NULL;
	char* filtersArray[FILTERS_NUM];
	int threadsArray[FILTERS_NUM];
	int filtersLen;
	int threadsLen;
	
	// getting the arguments using getopt
	while ((c = getopt (argc, argv, "f:t:")) != -1) {
		switch (c) {
			case 'f':
				filters = optarg;
				break;
			case 't':
				threads = optarg;
				break;
			case '?':
				fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
			default:
				return 1;
		}
	}
	
	// arguments checking
	if (!filters) {
		fprintf (stderr, "You must specify at least one filter.\n");
		return 1;
	}
	if (!threads) {
		fprintf (stderr, "You must specify how many threads will run per filter.\n");
		return 1;
	}
	if (optind == argc) {
		fprintf (stderr, "You must specify the input and output files.\n");
		return 1;
	}
	if (optind == argc-1) {
		fprintf (stderr, "You must specify the output file.\n");
		return 1;
	}

	filtersLen = parseFilters(filters, filtersArray);
	threadsLen = parseThreads(threads, threadsArray);
	
	if (filtersLen != threadsLen) {
		fprintf (stderr, "You must provide the same amount of filters ans threads.\n");
        return 1;
	}
	
	////////////////////////////
	//      DIR TREATMENT     //
	////////////////////////////
	
	char input[STRING_LEN];
	strncpy(input,argv[optind],CAT_LEN);
	char output[STRING_LEN];
	strncpy(output,argv[optind+1],CAT_LEN);
	
	int nameLen;

	struct dirent *dir;
	DIR *directory;
	int fileLen = 0;
	directory = opendir(input);
	if (directory != NULL) {
		while ((dir = readdir(directory)) != NULL && fileLen <= FILES_ARRAY_LEN) {
			nameLen = strlen (dir->d_name);
			if (nameLen >= 4) {
				if (strncmp (".bmp", &(dir->d_name[nameLen - 4]),4) == 0) {
					
					files[fileLen] = (char*)malloc(sizeof(char)*STRING_LEN+1);
					if(files[fileLen]==NULL){
						fprintf(stderr,"malloc n'initialise pas files[fileLen]");
						clean_malloc_file();
						exit(EXIT_FAILURE);
					}
					strncat(files[fileLen],input,CAT_LEN);
					strncat(files[fileLen],"/",CAT_LEN);
					strncat(files[fileLen],dir->d_name,CAT_LEN);
					save[fileLen] = (char*)malloc(sizeof(char)*STRING_LEN+1);
					if(save[fileLen]==NULL){
						fprintf(stderr,"malloc n'initialise pas save[fileLen]");
						clean_malloc_file();
						exit(EXIT_FAILURE);
					}
					strncat(save[fileLen],output,CAT_LEN);
					strncat(save[fileLen],"/",CAT_LEN);
					strncat(save[fileLen],dir->d_name,CAT_LEN);
					fileLen++;
				}
			}
		}
		if (fileLen > FILES_ARRAY_LEN) {
			fprintf(stderr,"Not enough space in files array. Please add more space\n");
			clean_malloc_file();
			exit(EXIT_FAILURE);
		}
		if (fileLen == 0){
			fprintf(stderr,"Images not found. Folder empty\n");
			clean_malloc_file();
			exit(EXIT_FAILURE);
		}
		closedir (directory);
	} else {
		fprintf(stderr,"folder not found");
		clean_malloc_file();
		exit(EXIT_FAILURE);
	}
	
	struct stat st = {0};
	
	if (stat(output, &st) == -1) { // check if outputDir already exists
		mkdir(output, 0700); // if not => new dir!
		printf(ANSI_COLOR_RED"Created folder"ANSI_COLOR_BLUE" %s\n"ANSI_COLOR_RESET,output);
	}
	
	//////////////////////////////
	//     IMAGES TREATMENT     //
	//////////////////////////////

	initShuffle(filtersArray, filtersLen);
	int block = fileLen/BLOCK;
	int modBlock = fileLen%BLOCK;
	int i;
	for (i=0; i<block; i++) {
		treatBlock(threadsArray,threadsLen, filtersArray,filtersLen, BLOCK, i);
		printf("\n");
	}
	treatBlock(threadsArray,threadsLen, filtersArray,filtersLen, modBlock, i);

	printf(ANSI_COLOR_RED"\nDone filtering"ANSI_COLOR_BLUE" %d"ANSI_COLOR_RED" images!!!\n"ANSI_COLOR_RESET,fileLen);
	
	clean_malloc_file();

	return 0;
}

	////////////////////////////
	//    ANNEXE FUNCTIONS    //
	////////////////////////////


int parseFilters(char str[], char* array[]) {
	char* tok;
	tok = strtok (str," [,]");
	
	int i = 0;
	while (tok != NULL) {
		array[i] = tok;
		i++;
		tok = strtok (NULL, " [,]");
	}
	return i;
}

int parseThreads(char str[], int array[]) {
	char* tok;
	tok = strtok (str," [,]");
	
	int i = 0;
	while (tok != NULL) {
		array[i] = (int) strtol(tok, (char **)NULL, 10);
		i++;
		tok = strtok (NULL, " [,]");
	}
	return i;
}

void treatBlock(int threadsArray[],int threadsLen, char* filtersArray[],int filtersLen, int nImage, int n) {
	
	int i;
	int err = 0;
	pthread_t threads[nImage];
	openArgs arg[nImage];
	
	for (i=0; i<nImage; i++) {
		arg[i].n = n*BLOCK+i;
		arg[i].img = &images[i];
		err = pthread_create(&threads[i],NULL,openImage,(void*)&arg[i]);
		if (err!=0)
			error(err,"pthread_create");
	}
	for (i=0; i<nImage; i++) {
		err = pthread_join(threads[i],NULL);
		if (err!=0)
			error(err,"pthread_create");
	}
	
	//////////////////////////////
	//     IMAGES TREATMENT     //
	//////////////////////////////
	
	int nPack;
	if (filtersLen == 1) {
		nPack = threadsArray[0];
	} else {
		nPack = maxArray(threadsArray,threadsLen);
	}
	int nBlur = numberBlur(filtersArray,filtersLen);
	initializeStatic(filtersLen,nPack*nImage,nBlur,nImage);
	imagesCut(images);
	int nBlurMat = initializeBlur(filtersArray);
	if (nBlur != nBlurMat) {
		error(1,"Issue with nBlur/nBlurMat\n");
	}
	initFilters(filtersArray);
	
	pthread_t threatment;
	printf("\n"ANSI_COLOR_RED"Images treatment"ANSI_COLOR_GREEN);
	err = pthread_create(&threatment,NULL,image_treatement,(void*)threadsArray);
	if (err!=0)
		error(err,"pthread_create in Main");
	err = pthread_join(threatment,NULL);
	printf(".\n\n"ANSI_COLOR_RESET);
	if (err!=0)
		error(err,"pthread_join in Main");
	free_mat_buf();
	freeStatic();
	printf(ANSI_COLOR_RED"Please wait. Waiting the images to be saved take a(n) (unusually) long time.\n\n");
	
	for (i=0; i<nImage; i++) {
		err = pthread_create(&threads[i],NULL,saveImage,(void*)&arg[i]);
		if (err!=0)
			error(err,"pthread_create");
	}
	for (i=0; i<nImage; i++) {
		err = pthread_join(threads[i],NULL);
		if (err!=0)
			error(err,"pthread_create");
	}
	
	cleanImage();
}

void* openImage(void* args) {
	
	openArgs* arg = (openArgs*)args;
	int num = arg->n;
	
	if (load_bmp(files[num], arg->img)) {
		error(-1,"Error calling load_bmp\n");
	}
	printf(ANSI_COLOR_RED"Opened image"ANSI_COLOR_BLUE" %d "ANSI_COLOR_RED": "ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET"\n", num+1, files[num]);
	pthread_exit(NULL);
}

void* saveImage(void* args) {
	
	openArgs* arg = (openArgs*)args;
	int num = arg->n;
	struct image* img = *arg->img;
	if (write_bmp(img, save[num])) {
		error(-1,"Error calling write_bmp\n");
	}
	printf(ANSI_COLOR_RED"Saved image"ANSI_COLOR_BLUE" %d "ANSI_COLOR_RED": "ANSI_COLOR_GREEN"%s"ANSI_COLOR_RESET"\n", num+1, save[num]);
	pthread_exit(NULL);
	
}

void cleanImage() {
	int i;
	for (i=0; i<BLOCK; i++) {
		if (images[i]!=NULL) {
			if (images[i]->pixels!=NULL)
				free(images[i]->pixels);
			free(images[i]);
		}
	}
}
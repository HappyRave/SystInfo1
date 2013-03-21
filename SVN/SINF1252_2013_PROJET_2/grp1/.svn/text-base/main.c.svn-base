#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "bitmap.h"
#include "filter.h"

#define FILES_ARRAY_LEN 256
#define CAT_LEN 128

int parseFilters(char str[], char* array[]);
int parseThreads(char str[], int array[]);
void applyFilters(struct image *img, int nThread, char* filter);

/* Exemple d'utilisation des fonctions fournies.
 * Vous devez impérativement changer le code ci-dessous.
 */
int main(int argc, char *argv[])
{
	
	// initializing needed variables
	char c;
    char *filters = NULL;
	char *threads = NULL;
	char* filtersArray[24];// DANGER, Pas trop? Pas trop peu?
	int threadsArray[24];
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
	if (optind == argc+1) {
        fprintf (stderr, "You must specify the output file.\n");
        return 1;
    }

	filtersLen = parseFilters(filters, filtersArray);
	threadsLen = parseThreads(threads, threadsArray);
	
	if (filtersLen != threadsLen) {
		fprintf (stderr, "You must provide the same amount of filters ans threads.\n");
        return 1;
	}
	
	char input[CAT_LEN];
	strncpy(input,argv[optind],CAT_LEN);
	char output[CAT_LEN];
	strncpy(output,argv[optind+1],CAT_LEN);
	puts(input);
	puts(output);
	
	struct timeval tv1,tv2;
	int nameLen;
	int fileLen = 0;
	struct image* images[FILES_ARRAY_LEN]; // 256 files? suffit?
	char* files[FILES_ARRAY_LEN];
	struct dirent *dir;
	DIR *directory;
	int lenIn = (int)strlen(input);
	
	directory = opendir(input);
	if (directory != NULL) {
		while ((dir = readdir(directory)) != NULL && fileLen <= FILES_ARRAY_LEN) {
			nameLen = strlen (dir->d_name);
			if (nameLen >= 4) {
				if (strncmp (".bmp", &(dir->d_name[nameLen - 4]),4) == 0) {
					files[fileLen] = dir->d_name;
					puts(files[fileLen-1]);
					puts(files[fileLen]);
					char* open = input;
					strncat(open, "/", CAT_LEN);
					strncat(open, dir->d_name, CAT_LEN);
					printf ("%s\n", open);
					if (load_bmp(open, &images[fileLen])) {
						fprintf(stderr,"Error calling load_bmp\n");
						return 1;
					}
					fileLen++;
					open[lenIn] = '\0';
				}
			}
		}
		closedir (directory);
		printf("len %d\n", fileLen);
		if (fileLen > FILES_ARRAY_LEN)
			err(1,"Not enough space in files array\nPlease add more space\n");
	} else {
		err(1,"folder not found");
	}
	
	struct stat st = {0};
	
	puts(output);
	if (stat(output, &st) == -1) { // check if outputDir already exists
		mkdir(output, 0700); // if not => new dir!
	}
	
	int j = 0;
	int k = 0;
	gettimeofday(&tv1,NULL);
	for (j = 0; j<fileLen; j++) {
		for (k=0; k<filtersLen; k++) {
			applyFilters(images[j],threadsArray[k],filtersArray[k]);
		}
	}
	gettimeofday(&tv2,NULL);
	int diff1=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
	printf("Temps d'éxécution est de %2d usec\n",diff1);
	
	int lenOut = (int)strlen(output);
	for (j = 0; j<fileLen; j++) {
		puts(files[j]);
		char* save = output;
		strncat(save, "/", CAT_LEN);
		strncat(save, files[j], CAT_LEN);
		puts(save);
		if (write_bmp(images[j], save)) {
			fprintf(stderr,"Error calling write_bmp\n");
			return 1;
		}
		save[lenOut] = '\0';
		puts(save);
	}
	
    return 0;
}

int parseFilters(char str[], char* array[]) {
	char* tok;
	tok = strtok (str," [,]");
	
	int i = 0;
	while (tok != NULL)
	{
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
	while (tok != NULL)
	{
		array[i] = (int) strtol(tok, (char **)NULL, 10);
		i++;
		tok = strtok (NULL, " [,]");
	}
	return i;
}

void applyFilters(struct image *img, int nThread, char* filter) {
	if (!strncasecmp(filter,"red",3)) {
		if (filter_red(img,nThread)) {
			fprintf(stderr,"Error calling filter_red\n");
			}
	} else if (!strncasecmp(filter,"green",5)) {
		if (filter_green(img,nThread)) {
			fprintf(stderr,"Error calling filter_green\n");
		}
	} else if (!strncasecmp(filter,"blue",4)) {
		if (filter_blue(img,nThread)) {
			fprintf(stderr,"Error calling filter_blue\n");
		}
	} else if (!strncasecmp(filter,"grayscale",9)) {
		if (filter_grayscale(img,nThread)) {
			fprintf(stderr,"Error calling filter_grayscale\n");
		}
	} else if (!strncasecmp(filter,"blur",4)) {
		if (filter_blur(img,nThread)) {
			fprintf(stderr,"Error calling filter_blur\n");
		}
	} else if (!strncasecmp(filter,"shuffle",7)) {
		if (filter_shuffle(img,nThread)) {
			fprintf(stderr,"Error calling filter_shuffle\n");
		}
	} else {
		fprintf(stderr,"Wrong filter format\n");
	}
}
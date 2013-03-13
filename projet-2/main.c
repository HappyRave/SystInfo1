#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "bitmap.h"
#include "filter.h"

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
	struct image *img;
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
	
	if (load_bmp(argv[optind], &img)) {
		fprintf(stderr,"Error calling load_bmp\n");
		return 1;
	}
	
	struct timeval tv1,tv2;
	int k = 0;
	for (k=filtersLen-1; k>=0; k--) {
		gettimeofday(&tv1,NULL);
		applyFilters(img,threadsArray[k],filtersArray[k]);
		gettimeofday(&tv2,NULL);
		int diff1=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
		printf("Temps d'éxécution est de %2d usec\n",diff1);
	}
	
	if (write_bmp(img, argv[optind+1])) {
		fprintf(stderr,"Error calling write_bmp\n");
		return 1;
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
	if (!strcasecmp(filter,"red")) {
		if (filter_red(img,nThread)) {
			fprintf(stderr,"Error calling filter_red\n");
			}
	} else if (!strcasecmp(filter,"green")) {
		if (filter_green(img,nThread)) {
			fprintf(stderr,"Error calling filter_green\n");
		}
	} else if (!strcasecmp(filter,"blue")) {
		if (filter_blue(img,nThread)) {
			fprintf(stderr,"Error calling filter_blue\n");
		}
	} else if (!strcasecmp(filter,"grayscale")) {
		if (filter_grayscale(img,nThread)) {
			fprintf(stderr,"Error calling filter_grayscale\n");
		}
	} else if (!strcasecmp(filter,"blur")) {
		if (filter_blur(img,nThread)) {
			fprintf(stderr,"Error calling filter_blur\n");
		}
	} else {
		fprintf(stderr,"Wrong filter format\n");
	}
}
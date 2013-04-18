#define _ISOC99_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/time.h>
#include <dirent.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "threadpool.h"
#include "bitmap.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_BOLD		   "\x1b[1m"
#define ANSI_BOLD_OFF	   "\x1b[21m"

/* Signature de fonction pour les filtres traitant des bandes d'images.
 * Chaque filtre prend une image en argument et
 * lui applique une opération. Retourne 0 en cas
 * de succès, -1 sinon.
 */
typedef int(filterBand_t)(struct image *img, int lineStart, int lineStop, struct image *read);

/* Retire la composante rouge de l'image */
filterBand_t filterBand_red;

/* Retire la composante verte de l'image */
filterBand_t filterBand_green;

/* Retire la composante bleue de l'image */
filterBand_t filterBand_blue;

/* Transforme l'image en niveau de gris en calculant
 * la moyenne sur les différentes composantes.
 */
filterBand_t filterBand_grayscale;

/* Ajoute de flou gaussien dans l'image en utilisant la
 * matrice carrée de taille 3 comme matrice de poids:
 *   0.0  0.2  0.0
 *   0.2  0.2  0.2
 *   0.0  0.2  0.0
 *
 * Si on considère la composante rouge de l'image suivante
 * (chaque element a une valeur entre 0 et 255) de taille
 * 6x5:
 *
 *	1  2  5  2  0  3
 *	   -------
 *	3 |2  5  1| 6  0	   0.0*2 + 0.2*5 + 0.0*1 +
 *	  |	      |
 *	4 |3  6  2| 1  4   ->  0.2*3 + 0.2*6 + 0.2*2 +   ->  3.2
 *	  |	      |
 *	0 |4  0  3| 4  2	   0.0*4 + 0.2*0 + 0.0*3
 *	   -------
 *	9  6  5  0  3  9
 *
 * La nouvelle valeur du pixel (3, 4) devient round(3.2) = 3.
 *
 * Lorsque les élements de la matrice se retrouvent en dehors
 * de l'image (c'est-à-dire pour les pixels à la bordure de l'image),
 * la valeur du pixel central sera utilisé pour les valeurs des pixels
 * en dehors des limites de l'image. C'est-à-dire, la nouvelle valeur
 * du pixel (0, 0) dans l'image précédente sera:
 *	0.2 * 0 + 0.2 * 9 + 0.2 * 6 + 0.2 * 9 + 0.2 * 9 = 6.6 -> 7
 */
filterBand_t filterBand_blur;

/* Mélange les couleurs de l'image de façon aléatoire
 * La composante rouge devient par exemple verte, etc
 */

filterBand_t filterBand_shuffle;

void initFilters(char* filter[]);
void apply_filter(elem_buf *band, int n);
struct pixel blur(struct image* img, struct pixel* pixels, int x, int y);
void shuffle(int *array);
void clean_malloc_file();
void initShuffle(char* filter[], int filterLen);

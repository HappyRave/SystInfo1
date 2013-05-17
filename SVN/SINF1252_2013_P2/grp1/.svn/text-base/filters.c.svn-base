#include "filterBand.h"

#define FILTERS_NUM 24

static char* filters[FILTERS_NUM];
static int colors[3] = {0,1,2};

void initShuffle(char* filter[], int filterLen){
	int i;
	for (i=0;i<filterLen;i++) {
		if (!strncasecmp(filter[i],"shuffle",7)) {
			shuffle(colors);
			break;
		}
	}
}

void initFilters(char* filter[]) {
	memcpy(filters,filter,sizeof(filters));
}

void apply_filter(elem_buf *band, int n) {
	int lineFirst = band->begin;
	int lineLast = band->finish;
	
	if (!strncasecmp(filters[n],"red",3)) {
		if (filterBand_red(band->img,lineFirst,lineLast, NULL)) {
			fprintf(stderr,"Error calling filter_red\n");
		}
	} else if (!strncasecmp(filters[n],"green",5)) {
		if (filterBand_green(band->img,lineFirst,lineLast, NULL)) {
			fprintf(stderr,"Error calling filter_green\n");
		}
	} else if (!strncasecmp(filters[n],"blue",4)) {
		if (filterBand_blue(band->img,lineFirst,lineLast, NULL)) {
			fprintf(stderr,"Error calling filter_blue\n");
		}
	} else if (!strncasecmp(filters[n],"grayscale",9)) {
		if (filterBand_grayscale(band->img,lineFirst,lineLast, NULL)) {
			fprintf(stderr,"Error calling filter_grayscale\n");
		}
	} else if (!strncasecmp(filters[n],"blur",4)) {
		if (filterBand_blur(band->img,lineFirst,lineLast, band->read_only)) {
			fprintf(stderr,"Error calling filter_blur\n");
		}
	} else if (!strncasecmp(filters[n],"shuffle",7)) {
		if (filterBand_shuffle(band->img,lineFirst,lineLast, NULL)) {
			fprintf(stderr,"Error calling filter_shuffle\n");
		}
	} else {
		fprintf(stderr,"Wrong filter format : %s\n", filters[n]);
	}
}

///////////////////
//      RED      //
/////////////////// 

int filterBand_red(struct image *img, int firstLine, int lastLine, struct image *read) {
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j].r = 0;
			}
		}
	}
	return 0;
}

///////////////////
//     GREEN     //
///////////////////

int filterBand_green(struct image *img, int firstLine, int lastLine, struct image *read) {
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j].g = 0;
			}
		}
	}
	return 0;
}

///////////////////
//      BLUE     //
///////////////////

int filterBand_blue(struct image *img, int firstLine, int lastLine, struct image *read) {
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j].b = 0;
			}
		}
	}
	return 0;
}

///////////////////
//      GRAY     //
///////////////////

int filterBand_grayscale(struct image *img, int firstLine, int lastLine, struct image *read) {
	int i;
	int j;
	int moy;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				moy = 0;
				moy += img->pixels[i * img->width +j].r;
				moy += img->pixels[i * img->width +j].g;
				moy += img->pixels[i * img->width +j].b;
				moy /= 3;
				img->pixels[i * img->width +j].r = moy;
				img->pixels[i * img->width +j].g = moy;
				img->pixels[i * img->width +j].b = moy;
			}
		}
	}
	return 0;
}

///////////////////
//      BLUR     //
///////////////////

int filterBand_blur(struct image *img, int firstLine, int lastLine, struct image *read) {
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j] = blur(img,read->pixels,j,i);
			}
		}
	}
	return 0;
}

///////////////////
//    SHUFFLE    //
///////////////////

int filterBand_shuffle(struct image *img, int firstLine, int lastLine, struct image *read) {
	int i;
	int j;
	int components[3];
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				components[0] = img->pixels[i * img->width +j].r;
				components[1] = img->pixels[i * img->width +j].g;
				components[2] = img->pixels[i * img->width +j].b;
				img->pixels[i * img->width +j].r = components[colors[0]];
				img->pixels[i * img->width +j].g = components[colors[1]];
				img->pixels[i * img->width +j].b = components[colors[2]];
			}
		}
	}
	return 0;
}

struct pixel blur(struct image* img, struct pixel* pixels, int x, int y) {
	double red = 0;
	double green = 0;
	double blue = 0;
	
	red=(double)pixels[y * img->width + x].r/5.0;
	green=(double)pixels[y * img->width + x].g/5.0;
	blue=(double)pixels[y * img->width + x].b/5.0;
	
	if (x != 0) {
		red+=(double)pixels[y * img->width + x - 1].r/5.0;
		green+=(double)pixels[y * img->width + x - 1].g/5.0;
		blue+=(double)pixels[y * img->width + x - 1].b/5.0;
	} else {
		red+=(double)pixels[y * img->width + x].r/5.0;
		green+=(double)pixels[y * img->width + x].g/5.0;
		blue+=(double)pixels[y * img->width + x].b/5.0;
	}
	
	if (x != img->width - 1) {
		red+=(double)pixels[y * img->width + x + 1].r/5.0;
		green+=(double)pixels[y * img->width + x + 1].g/5.0;
		blue+=(double)pixels[y * img->width + x + 1].b/5.0;
	} else {
		red+=(double)pixels[y * img->width + x].r/5.0;
		green+=(double)pixels[y * img->width + x].g/5.0;
		blue+=(double)pixels[y * img->width + x].b/5.0;
	}
	
	if (y != 0) {
		red+=(double)pixels[(y - 1) * img->width + x].r/5.0;
		green+=(double)pixels[(y - 1) * img->width + x].g/5.0;
		blue+=(double)pixels[(y - 1) * img->width + x].b/5.0;
	} else {
		red+=(double)pixels[y * img->width + x].r/5.0;
		green+=(double)pixels[y * img->width + x].g/5.0;
		blue+=(double)pixels[y * img->width + x].b/5.0;
	}
	
	if (y != img->height - 1) {
		red+=(double)pixels[(y + 1) * img->width + x].r/5.0;
		green+=(double)pixels[(y + 1) * img->width + x].g/5.0;
		blue+=(double)pixels[(y + 1) * img->width + x].b/5.0;
	} else {
		red+=(double)pixels[y * img->width + x].r/5.0;
		green+=(double)pixels[y * img->width + x].g/5.0;
		blue+=(double)pixels[y * img->width + x].b/5.0;
	}
	
	struct pixel pix = {(unsigned char)round(blue),(unsigned char)round(green),(unsigned char)round(red)};
	return pix;
}

void shuffle(int *array) {
	
	int i;
	srand ( time(NULL) );
	
	while (array[0] == 0 || array[1] == 1 || array[2] == 2) {
		for (i=0; i<3; i++)
			array[i] = rand() % 3;
	}
}

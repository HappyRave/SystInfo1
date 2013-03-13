#include "filter.h"
#include <pthread.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	struct image* img;
	struct pixel* pixels; // just for blur
	int firstLine;
	int lastLine;
	int line;
} args;

void* threadRed(void* argument);
void* threadGreen(void* argument);
void* threadBlue(void* argument);
void* threadGray(void* argument);
void* threadBlur(void* argument);
struct pixel blur(struct image* img, struct pixel* pixels, int x, int y);

/////////////////////////////////////////////////////////////
						 //RED//
/////////////////////////////////////////////////////////////

int filter_red(struct image *img, int nThread)
{
	int error = 0;
	int mod = img->height%nThread;
	if (mod > 0)
		mod = 1;
	
	pthread_t threads[nThread];
	args arguments[nThread];
	
	int i = 0;
	int band = (img->height)/nThread + mod;
	
	for (i=0; i<nThread; i++) {

		arguments[i].img = img;
		arguments[i].firstLine = i*band;
		arguments[i].lastLine = (i+1)*band-1;
		
		error=pthread_create(&threads[i],NULL,threadRed,(void*)&arguments[i]);
		if(error!=0)
			err(error,"pthread_create");
	}
	
	for (i=0; i<nThread; i++) {
		error=pthread_join(threads[i],NULL);
		if(error!=0)
			err(error,"pthread_join");
	}
	return 0;
}

void* threadRed(void* argument) {
	
	// unpacking arguments
	args* image = (args*)argument;
	struct image* img = image->img;
	int firstLine = image->firstLine;
	int lastLine = image->lastLine;
	
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j].r = 0;
			}
		}
	}
	pthread_exit(NULL);
}


/////////////////////////////////////////////////////////////
						//GREEN//
/////////////////////////////////////////////////////////////

int filter_green(struct image *img, int nThread)
{
	int error = 0;
	int mod = img->height%nThread;
	if (mod > 0)
		mod = 1;
	
	pthread_t threads[nThread];
	args arguments[nThread];
	
	int i = 0;
	int band = (img->height)/nThread + mod;
	
	for (i=0; i<nThread; i++) {
		
		arguments[i].img = img;
		arguments[i].firstLine = i*band;
		arguments[i].lastLine = (i+1)*band-1;
		
		error=pthread_create(&threads[i],NULL,threadGreen,(void*)&arguments[i]);
		if(error!=0)
			err(error,"pthread_create");
	}
	
	for (i=0; i<nThread; i++) {
		error=pthread_join(threads[i],NULL);
		if(error!=0)
			err(error,"pthread_join");
	}
	return 0;
}

void* threadGreen(void* argument) {
	
	// unpacking arguments
	args* image = (args*)argument;
	struct image* img = image->img;
	int firstLine = image->firstLine;
	int lastLine = image->lastLine;
	
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j].g = 0;
			}
		}
	}
	pthread_exit(NULL);
}

/////////////////////////////////////////////////////////////
						  //BLUE//
/////////////////////////////////////////////////////////////

int filter_blue(struct image *img, int nThread)
{
	int error = 0;
	int mod = img->height%nThread;
	if (mod > 0)
		mod = 1;
	
	pthread_t threads[nThread];
	args arguments[nThread];
	
	int i = 0;
	int band = (img->height)/nThread + mod;
	
	for (i=0; i<nThread; i++) {
		
		arguments[i].img = img;
		arguments[i].firstLine = i*band;
		arguments[i].lastLine = (i+1)*band-1;
		
		error=pthread_create(&threads[i],NULL,threadBlue,(void*)&arguments[i]);
		if(error!=0)
			err(error,"pthread_create");
	}
	
	for (i=0; i<nThread; i++) {
		error=pthread_join(threads[i],NULL);
		if(error!=0)
			err(error,"pthread_join");
	}
	return 0;
}

void* threadBlue(void* argument) {
	
	// unpacking arguments
	args* image = (args*)argument;
	struct image* img = image->img;
	int firstLine = image->firstLine;
	int lastLine = image->lastLine;
	
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j].b = 0;
			}
		}
	}
	pthread_exit(NULL);
}

/////////////////////////////////////////////////////////////
						 //GRAYSCALE//
/////////////////////////////////////////////////////////////

int filter_grayscale(struct image *img, int nThread)
{
	int error = 0;
	int mod = img->height%nThread;
	if (mod > 0)
		mod = 1;
	
	pthread_t threads[nThread];
	args arguments[nThread];
	
	int i = 0;
	int band = (img->height)/nThread + mod;
	
	for (i=0; i<nThread; i++) {
		
		arguments[i].img = img;
		arguments[i].firstLine = i*band;
		arguments[i].lastLine = (i+1)*band-1;
		
		error=pthread_create(&threads[i],NULL,threadGray,(void*)&arguments[i]);
		if(error!=0)
			err(error,"pthread_create");
	}
	
	for (i=0; i<nThread; i++) {
		error=pthread_join(threads[i],NULL);
		if(error!=0)
			err(error,"pthread_join");
	}
	return 0;
}

void* threadGray(void* argument) {
	
	// unpacking arguments
	args* image = (args*)argument;
	struct image* img = image->img;
	int firstLine = image->firstLine;
	int lastLine = image->lastLine;
	
	int moy;
	int i;
	int j;
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
	pthread_exit(NULL);
}

/////////////////////////////////////////////////////////////
				          //BLUR//
/////////////////////////////////////////////////////////////


int filter_blur(struct image *img, int nThread)
{
	int error = 0;
	int mod = img->height%nThread;
	if (mod > 0)
		mod = 1;
	
	pthread_t threads[nThread];
	args arguments[nThread];
	
	struct pixel* pixels = (struct pixel*)malloc(sizeof(struct pixel)*img->width*img->height);
	memcpy(pixels,img->pixels,sizeof(struct pixel)*img->width*img->height);
	
	int i = 0;
	int band = (img->height)/nThread + mod;
	
	for (i=0; i<nThread; i++) {
		
		arguments[i].img = img;
		arguments[i].firstLine = i*band;
		arguments[i].lastLine = (i+1)*band-1;
		arguments[i].pixels = pixels;
		
		error=pthread_create(&threads[i],NULL,threadBlur,(void*)&arguments[i]);
		if(error!=0)
			err(error,"pthread_create");
	}
	
	for (i=0; i<nThread; i++) {
		error=pthread_join(threads[i],NULL);
		if(error!=0)
			err(error,"pthread_join");
	}
	return 0;
}

void* threadBlur(void* argument) {
	
	// unpacking arguments
	args* image = (args*)argument;
	struct image* img = image->img;
	struct pixel* pixels = image->pixels;
	int firstLine = image->firstLine;
	int lastLine = image->lastLine;
	
	int i;
	int j;
	for (i=firstLine; i<=lastLine; i++) {
		if (i<img->height) {
			for (j=0; j<img->width; j++) {
				img->pixels[i * img->width +j] = blur(img,pixels,j,i);
			}
		}
	}
	pthread_exit(NULL);
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
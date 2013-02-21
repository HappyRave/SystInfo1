#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int *local(int n)
{
	int *array = (int*)malloc(sizeof(int)*n*n);
	int i;
	for (i=0; i<n*n; i++) {
		*(array+i)=1252;
	}
	return array;
}

int *nonLocal(int n)
{
	int *array = (int*)malloc(sizeof(int)*n*n);
	int i;
	int j;
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			*(array+j*n+i)=1252;
		}
	}
	return array;
}

int main(int argc, const char* argv[])
{
	int n=0;
	if (argc ==1) {
		n=10000;
	} else {
		n=atoi(argv[1]);
	}
	struct timeval tv1, tv2, tv3;
	int ret1=gettimeofday(&tv1,NULL);
	int *a=local(n);
	int ret2=gettimeofday(&tv2,NULL);
	int *a2=nonLocal(n);
	int ret3=gettimeofday(&tv3,NULL);
	
	int diff1=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
	int diff2=(tv3.tv_sec-tv2.tv_sec)*1000000+(tv3.tv_usec-tv2.tv_usec);
	printf("La durée d'éxécution de local(int n) est de %d usec\n",diff1);
	printf("La durée d'éxécution de nonLocal(int n) est de %d usec\n",diff2);

	double ratio=(double)diff2/(double)diff1;
	printf("En utilisant le principe de localité, la fonction local(int n) s'éxecute %f fois plus vite que nonLocal(int n)\n",ratio);
}
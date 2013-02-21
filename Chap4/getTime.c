#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define SIZE 10000

int matrix[SIZE][SIZE];

int sum() {
	int sum=0;
	int i;
	int j;
	for(i=0;i<SIZE;i++) {
		for(j=0;j<SIZE;j++) {
			sum+=matrix[i][j];
		}
	}
	return sum;
}

int sum2() {
	int sum=0;
	int i;
	int j;
	for(i=0;i<SIZE;i++) {
		for(j=0;j<SIZE;j++) {
			sum+=matrix[j][i];
		}
	}
	return sum;
}

int main(int argc, const char* argv[])
{
	struct timeval tv1,tv2,tv3;
	
	int ret1=gettimeofday(&tv1,NULL);
	int sumLine=sum();
	int ret2=gettimeofday(&tv2,NULL);
	int sumCol=sum2();
	int ret3=gettimeofday(&tv3,NULL);
	
	int diff1=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
	int diff2=(tv3.tv_sec-tv2.tv_sec)*1000000+(tv3.tv_usec-tv2.tv_usec);
	printf("Résultat de sum() est %d et la durée d'éxécution est de %2d usec\n",sumLine,diff1);
	printf("Résultat de sum2() est %d et la durée d'éxécution est de %2d usec\n",sumCol,diff2);
	double ratio=(double)diff2/(double)diff1;
	printf("Le calcul colonne par colonne prend %f fois plus de temps que le calcul ligne par ligne -> localité spatiale\n", ratio);
						
}


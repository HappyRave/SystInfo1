#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <err.h>

FILE* output;

int main(int argc, const char *argv[]) {
	FILE* file = fopen(argv[1],"r");
    if (file == NULL) err(-1,"No mesh file !");
	
	FILE* output = fopen(argv[2],"w+");
	
	int nCase;
	fscanf(file,"%d \n",&nCase);
	int ok;
	int i;
	
	for (i=0; i<nCase; i++) {
		int n;
		int m;
		fscanf(file,"%d %d \n",&n,&m);
		
		int** lawn = (int**)malloc(sizeof(int*)*(n+2));
		int j;
		for (j=0; j<n+2; j++) {
			lawn[j] = (int*)calloc((m+2),sizeof(int));
		}
		
		for (j=1; j<n+1; j++) {
			int k;
			for (k=1; k<m; k++) {
				fscanf(file,"%d ",&lawn[j][k]);
				//fprintf(output,"%d ",lawn[j][k]);
			}
			fscanf(file,"%d\n",&lawn[j][k]);
			//fprintf(output,"%d\n",lawn[j][k]);
		}
		
		ok = checkCase(lawn,n,m);
		if (ok == 0) {
			fprintf(output,"Case #%d: YES\n", i+1);
		} else {
			fprintf(output,"Case #%d: NO\n", i+1);
		}
		
		for (j=0; j<n+2; j++) {
			free(lawn[j]);
		}
		free(lawn);
	}
	
	fclose(file);
	fclose(output);
	return 0;
}

int checkCase(int** lawn, int n, int m) {
	
	int i;
	int j;
	int ok = 0;
	
	for (i=1; i<n+1; i++) {
		for (j=1; j<m+1; j++) {
			ok = checkCell(lawn,i,j,n,m);
			if (ok != 0) {
				return -1;
			}
		}
	}
	return 0;
}

int checkCell(int** lawn, int x, int y,int n, int m) {
	int h = lawn[x][y];
	int oka = checkH(h,x,y,lawn,n,m);
	int okb = checkV(h,x,y,lawn,n,m);
	
	
	if (oka+okb == -2) {
		return -1;
	} else {
		return 0;
	}
	
}

int checkH(int h, int x, int y , int** lawn, int n, int m) {
	int i;
	for (i=0; i<m+2; i++) {
		if (lawn[x][i]>h) {
			return -1;
		}
	}
	return 0;
}

int checkV(int h, int x, int y , int** lawn, int n, int m) {
	int i;
	for (i=0; i<n+2; i++) {
		if (lawn[i][y]>h) {
			return -1;
		}
	}
	return 0;
}
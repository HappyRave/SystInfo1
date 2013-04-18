#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <err.h>

FILE* output;

int main(int argc, const char *argv[]) {
	FILE* file = fopen(argv[1],"r");
    if (file == NULL) err(-1,"No mesh file !");
	
	output = fopen(argv[2],"w+");
	
	int nCase;
	fscanf(file,"%d \n",&nCase);
	
	int i;
	char** tik = (char**)malloc(sizeof(char*)*4);
	for (i=0; i<4; i++) {
		tik[i] = (char*)malloc(sizeof(char)*4);
	}
	int trash;
	for (i=0; i<nCase; i++) {
		int j;
		for (j=0; j<4; j++) {
			fscanf(file,"%c%c%c%c \n",&tik[0][j], &tik[1][j],&tik[2][j], &tik[3][j]);
			//printf("%c\n",tik[j][0]);
		}
		trash = checkCase(i,tik);
		fscanf(file,"\n");
	}
	
	for (i=0; i<4; i++) {
		free(tik[i]);
	}
	free(tik);
	
	fclose(file);
	fclose(output);
	return 0;
}

int checkCase(int n, char** tik) {
	
	int empty = checkEmpty(tik);
	int i;
	int ok;
	
	fprintf(output,"Case #%d: ", n+1);
	
	for (i=0; i<4; i++) {
		ok = checkCell(tik,i,0);
		if (ok == 0) {
			if (tik[0][0] != 'T') {
				fprintf(output,"%c won\n", tik[0][0]);
				return 0;
			} else {
				fprintf(output,"%c won\n", tik[0][1]);
				return 0;
			}
		}
		
		ok = checkCell(tik,0,i);
		if (ok == 0) {
			if (tik[0][0] != 'T') {
				fprintf(output,"%c won\n", tik[0][0]);
				return 0;
			} else {
				fprintf(output,"%c won\n", tik[0][1]);
				return 0;
			}
		}
	}
	
	if (empty == 0) {
		fprintf(output,"Game has not completed\n");
		return 0;
	} else {
		fprintf(output,"Draw \n");
		return 0;
	}
}

int checkCell(char** tik, int x, int y) {
	int ok;
	
	if (x == 0 && y == 0) {
		ok = checkLine(tik,x,y);
		if (ok == 0) return ok;
		
		ok = checkCol(tik,x,y);
		if (ok == 0) return ok;
		
		ok = checkDiag1(tik,x,y);
		if (ok == 0) return ok;
	}
	
	if (x == 0 && y != 0) {
		ok = checkLine(tik,x,y);
		if (ok == 0) return ok;
	}
	
	if (x != 0 && y == 0) {
		ok = checkCol(tik,x,y);
		if (ok == 0) return ok;
	}
	
	if ( x == 3 && y == 0) {
		ok = checkDiag2(tik,x,y);
		if (ok == 0) return ok;
	}

	return ok;
}

int checkLine(char** tik, int x, int y) {
	char c = tik[x][y];
	if (c == '.') {
		return -1;
	}
	if (c == 'T') {
		c = tik[x+1][y];
	}
	int ok = 0;
	int i;
	for (i=1; i<4 && ok == 0; i++) {
		if (tik[x+i][y] != c && tik[x+i][y] != 'T') {
			ok = -1;
		}
	}
	
	if (ok == 0) {
		tik[0][0] = c;
		tik[0][1] = tik[x+1][y];
	}

	return ok;
}

int checkCol(char** tik, int x, int y) {
	char c = tik[x][y];
	if (c == '.') {
		return -1;
	}
	if (c == 'T') {
		c = tik[x][y+1];
	}
	int ok = 0;
	int i;
	for (i=1; i<4 && ok == 0; i++) {
		if (tik[x][y+i] != c && tik[x][y+i] != 'T') {
			ok = -1;
		}
	}
	
	if (ok == 0) {
		tik[0][0] = c;
		tik[0][1] = tik[x][y+1];
	}
	
	return ok;
}

int checkDiag1(char** tik, int x, int y) {
	char c = tik[x][y];
	if (c == '.') {
		return -1;
	}
	if (c == 'T') {
		c = tik[x+1][y+1];
	}
	int ok = 0;
	int i;
	for (i=1; i<4 && ok == 0; i++) {
		if (tik[x+i][y+i] != c && tik[x+i][y+i] != 'T') {
			ok = -1;
		}
	}
	
	if (ok == 0) {
		tik[0][0] = c;
		tik[0][1] = tik[x+1][y+1];
	}

	return ok;
}

int checkDiag2(char** tik, int x, int y, char *one, char *two) {
	char c = tik[x][y];
	if (c == '.') {
		return -1;
	}
	if (c == 'T') {
		c = tik[x-1][y+1];
	}
	int ok = 0;
	int i;
	for (i=1; i<4 && ok == 0; i++) {
		if (tik[x-i][y+i] != c && tik[x-i][y+i] != 'T') {
			ok = -1;
		}
	}
	
	if (ok == 0) {
		tik[0][0] = c;
		tik[0][1] = tik[x-1][y+1];
	}
	
	return ok;
}

int checkEmpty(char** tik) {
	int i;
	int j;
	for (i=0; i<4; i++) {
		for (j=0; j<4; j++) {
			if (tik[i][j] == '.') {
				return 0;
			}
		}
	}
	return -1;
}
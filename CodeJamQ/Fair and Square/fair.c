#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <err.h>

int main(int argc, const char *argv[]) {
	FILE* file = fopen(argv[1],"r");
    if (file == NULL) err(-1,"No mesh file !");
	
	FILE* output = fopen(argv[2],"w+");
	
	int nCase;
	fscanf(file,"%d \n",&nCase);
	int ok;
	int i;
	
	for (i=0; i<nCase; i++) {
		long double a;
		long double b;
		fscanf(file,"%Lf %Lf \n",&a,&b);
		
		fprintf(output,"Case #%d: %d\n", i+1, checkCase(i,a,b));
	}
	fclose(file);
	fclose(output);
	return 0;
}

int checkCase(int n,long double a, long double b) {
	int f = 0;
	while (a<=b) {
		f += fair(a);
		a++;
	}
	printf("case #%d\n",n+1);
	return f;
}

int fair(long double l) {
	
	int lenl = len(l);
	int fair[lenl];
	int i;
	for (i=0; i<lenl; i++) {
		long double div = pow((long double)10,i+1);
		long double div2 = pow((long double)10,i);
		long double mod = fmod(l,div);
		fair[i] = mod/div2;
		l = l-mod;
	}
	
	for (i=0; i<lenl/2; i++) {
		if (fair[i] != fair[lenl-1-i]) {
			//printf("%d %d\n",fair[i],fair[lenl-1-i]);
			return 0;
		}
	}
	
	long double s = sqrt(l);
	long double c = l;
	//printf("%Lf\n",c);
	//printf("%Lf\n",s);
	if (fmod(s,(double long)1) != 0) {
		//printf("sqrt %Lf\n",s);
		return 0;
	}
	
	int lens = len(s);
	int square[lens];
	for (i=0; i<lens; i++) {
		long double div = pow((long double)10,i+1);
		long double div2 = pow((long double)10,i);
		long double mod = fmod(s,div);
		square[i] = mod/div2;
		s = s-mod;
	}
	
	for (i=0; i<lenl/2; i++) {
		if (fair[i] != fair[lenl-1-i]) {
			//printf("%d %d\n",fair[i],fair[lenl-1-i]);
			return 0;
		}
	}
	
	for (i=0; i<lens/2; i++) {
		if (square[i] != square[lens-1-i]) {
			//printf("sqrt %d %d\n",square[i],square[lenl-1-i]);
			return 0;
		}
	}
	
	//puts("yo");
	return 1;
	
}

int len(long double value){
	int l=1;
	while(value>9){ l++; value/=10; }
	return l;
}

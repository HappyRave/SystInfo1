#include <stdlib.h>
#include <stdio.h>

/*strlen - return the length of a string, ignore terminator char '\0'
 @input - char*
 @ouput - int (length of char*)
 */

int strlen(const char *s)
{
    int i=0;
    
    while (s[i] != '\0') {
        i++;
    }
    return i;
}

/*strncat - concatenate the first n char of a src string with a dest string
 @input - dest char*, src char*, int)
 @output - string* composed of string dest and the n first char of string src
 */

char *strncat(char *dest, const char *src, int n)
{
	int dLength=strlen(dest);
	int sLength=strlen(src);
	if (n<sLength) {
		char *target=malloc(n+dLength+1);
		int i;
		for (i=0; i<=dLength; i++) {
			target[i]=dest[i];
		}
		int j;
		for (j=0; j<n; j++) {
			target[dLength+j]=src[j];
		}
		target[i+j-1]='\0';
		return target;
	} else {
		int i;
		char *target=malloc(dLength+sLength+1);
		for (i=0; i<=dLength; i++) {
			target[i]=dest[i];
		}
		int j;
		for (j=0; j<=sLength; j++) {
			target[dLength+j]=src[j];
		}
		target[i+j-1]='\0';
		return target;
	}
}

/*strncat - concatenate a src string with a dest string
 @input - dest char*, src char*)
 @output - string* composed of string dest and string src
 */

char *strcat(char *dest, const char *src)
{
	return strncat(dest,src,2147483646);
}

/*strcasecmp - compare 2 strings ignoring case
 @intput - *s1 and *s2, two string to be compared
 @output - an integer less than, equal to, or greater than zero if s1 is found, respectively, to be less than, to match, or be greater than s2.  
 */

int strcasecmp(const char *s1, const char *s2)
{
	int s1Length = strlen(s1);
	int s2Length = strlen(s2);
	
	int i;
	int totS1=0;
	for (i=0; i<=s1Length; i++) {
		totS1+=tolower(s1[i]);
	}
	
	int j;
	int totS2=0;
	for (j=0; j<=s1Length; j++) {
		totS2+=tolower(s2[j]);
	}
	
	return totS1-totS2;
	
}
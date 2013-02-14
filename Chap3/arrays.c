#include <stdlib.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
  int a[3][3]={1,2,3,4,5,6,7,8,9};
  printf("%d\n", a[0][2]);
  printf("Le resultat de a[0][2] sur a[3][3]={1,2,3,4,5,6,7,8,9} est 3.\n");
  printf("Ca veut dire que les élements sont stockés ligne par ligne!\n");
}

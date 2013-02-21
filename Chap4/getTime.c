#include <stdlib.h>
#include <stdio.h>

#define SIZE 10000

int matrix[SIZE][SIZE];

int sum() {
  int sum=0;
  for(int i=0;i<SIZE;i++) {
    for(int j=0;j<SIZE;j++) {
      sum+=matrix[i][j];
    }
  }
  return sum;
}

int sum2() {
  int sum=0;
  for(int i=0;i<SIZE;i++) {
    for(int j=0;j<SIZE;j++) {
      sum+=matrix[j][i];
    }
  }
  return sum;
}

int main(int argc, const char* argv[])
{

}


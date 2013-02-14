#include <stdlib.h>
#include <stdio.h>

struct vector_t {
  int size;
  float *v;
};
// initialise le vecteur à la valeur du réel
struct vector_t * init(int, float) ;
// récupère le nième élément
float get(struct vector_t *, int) ;
// fixe la valeur du nième élément
void set(struct vector_t *, int , float);
// supprime un vecteur
void destroy(struct vector_t *);

struct vector_t* init(int i, float f)
{
  float* flo=(float*)malloc(sizeof(float)*i);
  int j;
  for (j=0; j<i;j++){
    *(flo+j)=f;
  }
  struct vector_t* vector=(struct vector_t*)malloc(sizeof(struct vector_t));
  vector->size=i;
  vector->v=flo;
  //free(flo);
  return vector;
}

float get(struct vector_t *vec, int i)
{
  float* elem=vec->v;
  float r=*(elem+i-1);
  return r;
}

void set(struct vector_t *vec, int i, float f)
{
  float* elem=vec->v;
  *(elem+i-1)=f;
  //vec->v=elem;
  //free(elem);
}

void destroy(struct vector_t *vec)
{
  free(vec->v);
  free(vec);
}

int main(void)
{
  struct vector_t* vector=init(7,2.0);
  float f=get(vector,3);
  printf("Element 3 est %f\n", f);
  set(vector,2,7.0);
  printf("Element 2 de vector est %f\n", get(vector,2));
  destroy(vector);
}


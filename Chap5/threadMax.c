#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int launcher(int nMax);
void* f(void* i);

int main(int argc, const char* argv[])
{
	int ret=launcher(1000000);
	exit(0);
}

int launcher(int nMax)
{
	int i;
	pthread_t t;
	int err=0;
	for (i=1; i<=nMax && err == 0; i++) {
		err = pthread_create(&t,NULL,&f,(void*)&i);
	}
	printf("Max number of threads: %d\n",i-1);
	pthread_cancel(t);
	return i-1;
}

void* f(void* i)
{
	printf("%d\n", *((int*)i));
	sleep();
}

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <pthread.h>


typedef struct {
	int* a;
	pthread_mutex_t* mutex;
} args;

void test(int* a, pthread_mutex_t* mutex);
void* testThread(void* arguments);

int main(int argc, const char* argv[])
{
	int error = 0;
	int a = 2;
	pthread_mutex_t mutex;
	pthread_t thread;

	error=pthread_mutex_init( &mutex, NULL);
	if(error!=0)
		err(error,"pthread_mutex_init");
	
	args arguments = {&a, &mutex};
	
	error=pthread_create(&thread,NULL,testThread,(void*)&arguments);
	if(error!=0)
		err(error,"pthread_create");
	
	pthread_join(thread,NULL);
	
	printf("val = %d\n", a);
	return error;
}

void test(int* a, pthread_mutex_t* mutex) {
	int error = 0;
	
	error=pthread_mutex_lock(mutex);
	if(error!=0)
		err(error,"pthread_mutex_lock");
	//error=pthread_mutex_lock(mutex);
	if(error!=0)
		err(error,"pthread_mutex_lock Second Lock :-(");
	
	*a += 1;
	
	error=pthread_mutex_unlock(mutex);
	if(error!=0)
		err(error,"pthread_mutex_unlock");
	
	error=pthread_mutex_unlock(mutex);
	if(error!=0)
		err(error,"pthread_mutex_unlock Second Unlock :-(");

}

void* testThread(void* arguments) {
	args* val = (args*)arguments;
	test(val->a,val->mutex);
	return NULL;
}
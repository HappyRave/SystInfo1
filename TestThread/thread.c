#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
void *foo_Thread(void *input) {
    char *message;
    message = (char*)input;
    printf("Inside  %s\n", message);
}
int main(void) {
    pthread_t thread1, thread2;
    char *str1 = "Thread one";
    char *str2 = "Thread two";
    int  ret1, ret2;
    //Create independent threads each of which will execute function
    ret1 = pthread_create( &thread1, NULL, foo_Thread, (void*) str1);
    ret2 = pthread_create( &thread2, NULL, foo_Thread, (void*) str2);
    // Wait till threads are complete
     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL);
     printf("Thread one returns: %d\n",ret1);
     printf("Thread two returns: %d\n",ret2);
     exit(0);
}

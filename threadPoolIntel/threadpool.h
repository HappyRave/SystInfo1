#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

/*
 * Task structure
 * A task to be executed by a thread
 */

typedef struct _pthread_task
{
	void (*routine)(); // task to be executed
	void *data; // data for the execution
	struct _pthread_task *next; // the next task in the queue
} pthread_task_t;

/*
 * Execute a task in the queue
 */

int pthread_task_exec(pthread_task_t *task);

/*
 * Structure to store pending tasks
 * When a task is waiting to be executed, it is stored here
 * A mutex synchronize the acces to the tasks
 */

typedef struct _pthread_queue
{
	pthread_mutex_t mutex; // synchronize acces
	pthread_cond_t cond; // condition used by threads to wiat
	pthread_task_t *first; // first task in the queue
	pthread_task_t *last; // last task
	int terminated; // flag for termination of the queue
} pthread_queue_t;

/*
 * function used to add tasks on the bottom of the queue
 * accesses to the queue are synchronized my a mutex
 */

int pthread_queue_push(pthread_queue_t *queue, const pthread_task_t *task);

/*
 * function used to remove a task on top of the queue
 */

int pthread_queue_pop(pthread_queue_t *queue, pthread_task_t *task);

/*
 * the interesting part:
 * the struct used to contain the threadpool
 */

typedef struct _pthread_pool
{
	pthread_mutex_t mutex; // mutex used to the synchronized acces to the pool
	pthread_queue_t queue; // the queue where the tasks are stored
	pthread_t *threads; // the threads array
	size_t thread_init; // initialized threads number
	size_t thread_count; // total threads number
} pthread_pool_t;

/*
 * give to a threadpool a task to be executed
 * place this task in the queue
 */

int pthread_pool_exec(pthread_pool_t *thread_pool, void (*function)(), void *data);

/*
 * fais executer une routine par le threadpool
 */

void *pthread_pool_routine(void *data);

void pthread_pool_constructor() __attribute__((constructor));
void pthread_pool_destructor()  __attribute__((destructor));

int pthread_async_exec(void (*function)(), void *data);

#endif
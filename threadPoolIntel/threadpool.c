#include "threadpool.h"

int pthread_task_exec(pthread_task_t *task)
{
	if (task == NULL)
    {
		return (-1);
    }
	if (task->data == NULL)
    {
		task->routine();
    }
	else
    {
		task->routine(task->data);
    }
	return (0);
}

int pthread_queue_push(pthread_queue_t *queue, const pthread_task_t *task)
{
	pthread_mutex_t *mutex;
	pthread_cond_t *cond;
	pthread_task_t *qtask;
	
	if ((queue == NULL) || (task == NULL))
    {
		return (-1);
    }
	if ((qtask = malloc(sizeof(*qtask))) == NULL)
    {
		return (-1);
    }
	pthread_task_init(qtask, task->routine, task->data);
	mutex = &(queue->mutex);
	cond = &(queue->cond);
	pthread_mutex_lock(mutex);
	if (queue->last == NULL)
    {
		queue->first = qtask;
		queue->last = qtask;
    }
	else
    {
		queue->last->next = qtask;
		queue->last = qtask;
    }
	pthread_cond_signal(cond);
	pthread_mutex_unlock(mutex);
	return (0);
}

int pthread_queue_pop(pthread_queue_t *queue, pthread_task_t *task)
{
	pthread_mutex_t *mutex;
	pthread_cond_t *cond;
	pthread_task_t *qtask;
	
	if ((queue == NULL) || (task == NULL))
    {
		return (-1);
    }
	mutex = &(queue->mutex);
	cond = &(queue->cond);
	pthread_mutex_lock(mutex);
	while (queue->first == NULL)
    {
		pthread_cond_wait(cond, mutex);
		if (queue->terminated)
		{
			pthread_mutex_unlock(mutex);
			return (-1);
		}
    }
	qtask = queue->first;
	queue->first = queue->first->next;
	pthread_mutex_unlock(mutex);
	pthread_task_init(task, qtask->routine, qtask->data);
	pthread_task_destroy(qtask);
	free(qtask);
	return (0);
}

int pthread_pool_exec(pthread_pool_t *thread_pool, void (*function)(), void *data)
{
	pthread_task_t task;
	int code;
	
	if ((thread_pool == NULL) || (function == NULL))
    {
		return (-1);
    }
	pthread_task_init(&task, function, data);
	code = pthread_queue_push(&(thread_pool->queue), &task);
	pthread_task_destroy(&task);
	return (code);
}

void *pthread_pool_routine(void *data)
{
	pthread_pool_t *thread_pool = (pthread_pool_t *)data;
	pthread_task_t task;
	
	pthread_mutex_lock(&(thread_pool->mutex));
	thread_pool->thread_init += 1;
	pthread_mutex_unlock(&(thread_pool->mutex));
	while (pthread_queue_pop(&(thread_pool->queue), &task) == 0)
    {
		pthread_task_exec(&task);
    }
	return (NULL);
}

static pthread_pool_t thread_pool;

void pthread_pool_constructor()
{
	if (pthread_pool_init(&thread_pool) < 0)
    {
		fprintf(stderr, "ERROR: failed to create the thread pool, %sn", strerror(errno));
    }
}

void pthread_pool_destructor()
{
	pthread_pool_destroy(&thread_pool);
}

int pthread_async_exec(void (*function)(), void *data)
{
	return (pthread_pool_exec(&thread_pool, function, data));
}
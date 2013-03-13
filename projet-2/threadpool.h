/** 
 * threadpool.h
 * 
 * Created on 9/03/2013
 * Author: Maxime De Mol (NOMA 21941100)
 *
 * Header for my threadpool implementation
 *
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_


/**
 * @brief Definition of a structure representing a threadpool
 *
 * @attribute tasks		The queue containing the task to be executed
 * @attribute threads	The array of threads
 * @attribute next		The next task in the queue that stores the tasks.
 */

typedef struct {
	queue_t tasks;
	p_thread *threads;
	int nThreads;
	int stopFlag;
	pthread_mutex_t mutex;
} threadpool_t;

#endif
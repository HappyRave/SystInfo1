#include "threadpool.h"

void routine(int i)
{
	printf("%d\n",i);
}

int main()
{
	int i;
	
	for (i = 0; i < 20; ++i)
    {
		pthread_async_exec(routine, NULL);
    }
	return (0);
}
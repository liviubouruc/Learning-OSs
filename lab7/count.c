#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
#include <errno.h>

#define MAX_RESOURCES 5

int available_resources = MAX_RESOURCES;

pthread_mutex_t mtx;
pthread_t *vthr;

int decrease_count(int count)
{
    pthread_mutex_lock(&count);
    if(available_resources < count) 
    {
        pthread_mutex_unlock(&count);
        return -1;
    }
    else 
    {
        available_resources -= count;
        pthread_mutex_unlock(&count);
        printf("Got %d resources %d remaining\n", count, available_resources);
    }
    return 0;
}

int increase_count(int count)
{
    pthread_mutex_lock(&count);
    available_resources += count;
    printf("Released %d resources %d remaining\n", count, available_resources);
    pthread_mutex_unlock(&count);
    return 0;
}

void *f(void *argc)
{
    int *val = (int *)argc;
    int cpy = *val;
    while(decrease_count(cpy)==-1) sleep(1);
    increase_count(cpy);
    return NULL;
}

int main()
{
    if(pthread_mutex_init(&mtx, NULL))
	{
	    perror(NULL);
	    return errno;
	}
  
    vthr = (pthread_t *)malloc(5*sizeof(pthread_t));
    for(int i = 1; i <= 5; ++i)
    {
        int *val;
        val=(int *)malloc(sizeof(int));
        *val=i;
        if(pthread_create(&vthr[i], NULL, f, val))
        {
            perror(NULL);
            return errno;
        }
    }

    for(int i = 1; i <= 5; ++i)
    {
        if(pthread_join(vthr[i], NULL))
        {
            perror(NULL);
            return errno;
        }
        pthread_mutex_destroy(&mtx);
    }
    return 0;
}
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
#include <semaphore.h>

pthread_mutex_t mtx;
sem_t sem;
int nrt;

void init(int n)
{
    nrt = n;
    if(pthread_mutex_init(&mtx, NULL))
    {
        perror(NULL);
        return errno;
    }	
}

void barrier_point()
{
    pthread_mutex_lock(&mtx);
    nrt--;
    if(!nrt)
    {
        sem_post(&sem);
        return;	
    }
    pthread_mutex_unlock(&mtx); 
    sem_wait(&sem);
    sem_post(&sem);
}

void *tfun(void *v)
{
    int *tid=(int *)v;
    printf("%d reached the barrier\n", *tid);
    barrier_point();
    printf("%d passed the barrier\n", *tid);
    free(tid);
    return NULL;
}

int main()
{
    if(sem_init(&sem, 0, 0))
    {
        perror(NULL);
        return errno;
    }

    init(5);
    printf("NTHRS=%d\n", nrt);
    int cpy_nr = nrt;

    pthread_t *vthr;
    vthr = (pthread_t *)malloc(nrt*sizeof(pthread_t));
    for(int i = 0; i < cpy_nr; ++i)
    {
        int *v;
        v = (int *)malloc(sizeof(int));
        *v=i;
        if(pthread_create(&vthr[i], NULL, tfun, v))
        {
            perror(NULL);
            return errno;
        } 
    }

    for(int i = 0; i < cpy_nr; ++i)
    {
        if(pthread_join(vthr[i], NULL))
        {
            perror(NULL);
            return errno;	
        }
    }

    pthread_mutex_destroy(&mtx);
    sem_destroy(&sem);
    return 0;
}
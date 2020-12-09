#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

int n, m, p, a[10][10], b[10][10], sol[10][10];
struct pozitie
{
        int i, j;
};

void *calc(void *args)
{
        struct pozitie *aux = args;
        int *nr = (int*) malloc(sizeof(int));
        for(int k = 1; k <= m; ++k) (*nr) += a[aux->i][k] * b[k][aux->j];
        return nr;
}

int main()
{
        pthread_t thr[105];        

        printf("n = ");
        scanf("%d", &n);
        printf("m = ");
        scanf("%d", &m);
        printf("Introdu matricea 1 (n x m):\n");
        for(int i = 1; i <= n; ++i)
                for(int j = 1; j <= m; ++j) scanf("%d", &a[i][j]);
        printf("p = ");
        scanf("%d", &p);
        printf("Introdu matricea 2 (m x p):\n");
        for(int i = 1; i <= m; ++i)
                for(int j = 1; j <= p; ++j) scanf("%d", &b[i][j]);

        int k = 1;
        for(int i = 1; i <= n; ++i)
                for(int j = 1; j <= p; ++j)
                {
                        struct pozitie *poz = (struct pozitie*) malloc(sizeof(struct pozitie));
                        poz->i = i; poz->j = j;
                        if(pthread_create(&thr[k++], NULL, calc, poz))
                        {
                                perror(NULL);
                                return errno;
                        }
                }

        k = 1;
        for(int i = 1; i <= n; ++i)
                for(int j = 1; j <= p; ++j)
                {
                        int *result;
                        if(pthread_join(thr[k++], &result))
                        {
                                perror(NULL);
                                return errno;
                        }
                        sol[i][j] = *result;
                }
        for(int i = 1; i <= n; ++i)
        {
                for(int j = 1; j <= p; ++j) printf("%d ", sol[i][j]);
                printf("\n");
        }
        return 0;
}
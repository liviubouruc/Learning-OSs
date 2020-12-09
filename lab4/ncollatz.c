#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char** argv)
{
        int n = argc-1;
        int *v = malloc(sizeof(int) * argc);
        for(int i = 0; i < n; ++i)
                v[i] = atoi(argv[i+1]);

        printf("Starting parent %d\n", getpid());
        for(int i = 0; i < n; ++i)
        {
                pid_t pid = fork();
                if(pid < 0) return errno;
                if(pid == 0)
                {
                        int x = v[i];
                        printf("%d: %d ", x, x);
                        while(x != 1)
                        {
                                if(x % 2 == 1) x = x*3 + 1;
                                else x = x / 2;
                                printf("%d ", x);
                        }
                        printf("\nDone Parent %d Me %d\n", getppid(), pid);
                        exit(0);
                }
        }
        for(int i = 0; i < n; ++i) wait(NULL);
        printf("Done Parent %d Me %d\n", getppid(), getpid());
        return 0;
}
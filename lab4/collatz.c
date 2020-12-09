#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char** argv)
{
        int n = atoi(argv[1]);
        pid_t pid = fork();
        if(pid < 0) return errno;
        else if(pid == 0)
        {
                printf("%d: %d ", n, n);
                while(n != 1)
                {
                        if(n % 2 == 1) n = n*3 + 1;
                        else n = n/2;
                        printf("%d ", n);
                }
                printf("\n");
        }
        else if(pid > 0)
        {
                wait(NULL);
                printf("Child %d finished", pid);
        }
        return 0;
}

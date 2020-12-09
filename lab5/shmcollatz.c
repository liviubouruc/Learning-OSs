#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <errno.h>

int main(int argc, char **argv)
{
    printf("Starting parent %d\n", getpid());

    char shm_name[] = "myshm";
    int shm_fd;
    shm_fd = shm_open(shm_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(shm_fd < 0)
    {
        perror(NULL);
        return errno;
    }

    size_t shm_size = 1000;
    if(ftruncate(shm_fd, shm_size) == -1)
    {
        perror(NULL);
        shm_unlink(shm_name);
        return errno;
    }

    int *shm_ptr = mmap(0, shm_size, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED)
    {
        perror(NULL);
        shm_unlink(shm_name);
        return errno;
    }

    for(int i = 1; i < argc; ++i)
    {
        pid_t pid = fork();
        if(pid < 0) return errno;
        if(pid == 0)
        {
            printf("Done Parent %d Me %d\n", getppid(), getpid());
            int x = atoi(argv[i]);
            char *mem = shm_ptr + (i-1)*(shm_size/(argc-1));
            int aux = sprintf(mem, "%d: %d ", x, x);
            while(x != 1)
            {
                if(x % 2 == 0) x /= 2;
                else x = x*3 + 1;
                aux += sprintf(mem+aux, "%d ", x);
            }
            *(mem+aux)="NULL";
            exit(0);
        }
    }
    for(int i = 0; i < argc; ++i) wait(NULL);
    for(int i = 1; i < argc; ++i) printf("%s\n", shm_ptr + (i-1)*(shm_size/(argc-1)));
    printf("Done Parent %d Me %d\n", getppid(), getpid());
    
    munmap(shm_ptr, shm_size);
    shm_unlink(shm_name);
    return 0;
}
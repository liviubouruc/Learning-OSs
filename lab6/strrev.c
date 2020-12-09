#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

void *inv(void *args)
{
        char *s = args;
        char *copy = (char*) malloc(strlen(s));
        int l = 0;
        for(int i = strlen(s)-1; i >= 0; --i) copy[l++] = s[i];
        return copy;
}

int main(int argc, char *argv[])
{
        char *s = argv[1];
        pthread_t thr;
        if(pthread_create(&thr, NULL, inv, s))
        {
                perror(NULL);
                return errno;
        }

        char *sol;
        if(pthread_join(thr, &sol))
        {
                perror(NULL);
                return errno;
        }
        printf("%s\n", sol);
        return 0;
}
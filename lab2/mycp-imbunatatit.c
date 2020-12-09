#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
	if(argc != 3)
	{
		printf("Prea multe argumente. Exemplu: ./mycp foo bar");
		return 0;
	}

	char* src = argv[1];
	char* dest = argv[2];
	
	int auxSrc = open(src, O_RDONLY);
	if(auxSrc == -1)
	{
		printf("Eroare: %d", errno);
		return 0;
	}
	int auxDest = open(dest, O_CREAT | O_WRONLY, S_IRWXU);
	if(auxDest == -1)
	{
		printf("Eroare: %d", errno);
		return 0;
	}

	ssize_t reader = -1;
	char buffer[1024];
	while(reader = read(auxSrc, buffer, 1024))
	{
		int written = 0, writer;
		while(writer = write(auxDest, buffer+written, reader-written)) written += writer;
		if(written == -1)
		{
			printf("Eroare: %d", errno);
			return 0;
		}
	}
	if(reader == -1)
	{
		printf("Eroare: %d", errno);
		return 0;
	}

	close(auxSrc);
	close(auxDest);
	return 0;
}

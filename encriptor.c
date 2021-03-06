#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void make_perm(int v[], int n) 
{ 
	srand(time(NULL)); 
	for(int i = n-1; i; --i) 
	{ 
		int j = rand() % (i+1); 
		int aux = v[i];
        v[i] = v[j];
        v[j] = aux; 
	} 
}

int main(int argc, char *argv[])
{
    int page_size = getpagesize();
    if(argc == 2)
    {
        char permsFile[] = "permutari";
        char writeFile[strlen(argv[1])];   
        strcpy(writeFile, argv[1]);
        strcpy(writeFile+strlen(argv[1])-4, "Encripted.txt");

        FILE *fin = fopen(argv[1], "r");
        if(fin == NULL)
        {
            perror("Errore la deschiderea fisierului de citire");
            return 0;
        }

        FILE *foutPerms = fopen(permsFile, "w");
        if(foutPerms == NULL)
        {
            perror("Eroare la deschiderea fisierului de permutari");
            return 0;
        }

        FILE *fout = fopen(writeFile, "w");
        if(fout == NULL)
        {
            perror("Eroare la deschiderea fisierului de scriere");
            return 0;
        }

        int nrcuv = 1;
        char c;
        while((c = fgetc(fin)) != EOF)
            if(c == ' ') nrcuv++;
        fseek(fin, 0, SEEK_SET);

        char shm_name[] = "shmem";
        int shm_fd = shm_open(shm_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if(shm_fd < 0)
        {
            perror(NULL);
            return 0;
        }
        size_t shm_size = page_size * nrcuv;
        if(ftruncate(shm_fd, shm_size) == -1)
        {
            perror(NULL);
            shm_unlink(shm_name);
            return 0;
        }

        char *shm_ptr;
        for(int i = 0; i < nrcuv; ++i)
        {
            shm_ptr = mmap(0, page_size, PROT_WRITE, MAP_SHARED, shm_fd, page_size*i);
            if(shm_ptr == MAP_FAILED)
            {
                perror(NULL);
                shm_unlink(shm_name);
                return errno;
            }

            while((c = fgetc(fin)) != EOF)
				if(c == ' ') break;
				else shm_ptr += sprintf(shm_ptr, "%c", c);
        }

        for(int i = 0; i < nrcuv; ++i)
        {
            shm_ptr = mmap(0, page_size, PROT_READ, MAP_SHARED, shm_fd, page_size*i);
            pid_t pid = fork();
            if(pid < 0) return errno;
            else if(pid == 0)
            {
                int len_cuv = strlen(shm_ptr);
				int perm[len_cuv];
				for(int j = 0; j < len_cuv; ++j) perm[j] = j;
				make_perm(perm, len_cuv);
                
				char ecripted[len_cuv+1];
				for(int j = 0; j < len_cuv; ++j) ecripted[j] = shm_ptr[perm[j]];
				
				for(int j = 0; j < len_cuv; ++j)
				{
					fprintf(foutPerms, "%d ", perm[j]);
					fprintf(fout, "%c", ecripted[j]);
				}
                if(i != nrcuv-1)
                {
                    fprintf(foutPerms, "\n");
                    fprintf(fout, " ");
                }
				exit(0);
            }
            else wait(NULL);
            munmap(shm_ptr, page_size);
        }

        printf("Fisierul a fost criptat cu succes!\n");
        shm_unlink(shm_name);
        fclose(fin);
        fclose(fout);
        fclose(foutPerms);
    }
    else if(argc == 3)
    {
        char writeFile[strlen(argv[1])];
        strcpy(writeFile, argv[1]);
        strcpy(writeFile+strlen(argv[1])-4, "Decripted.txt");

        FILE *fin = fopen(argv[1], "r");
        if(fin == NULL)
        {
            perror("Errore la deschiderea fisierului de citire");
            return 0;
        }

        FILE *finPerms = fopen(argv[2], "r");
        if(finPerms == NULL)
        {
            perror("Eroare la deschiderea fisierului de permutari");
            return 0;
        }

        FILE *fout = fopen(writeFile, "w");
        if(fout == NULL)
        {
            perror("Eroare la deschiderea fisierului de scriere");
            return 0;
        }

        int nrcuv = 1, len = 0, maxLen = 0;
        char c;
        while((c = fgetc(fin)) != EOF)
        {
            if(c == ' ')
            {
                nrcuv++;
                if(len > maxLen) maxLen = len;
                len = 0;
                continue;
            }
            len++;
        }
        fseek(fin, 0, SEEK_SET);
        int perms[nrcuv+1][maxLen+1];

        char shm_name[] = "shmem";
        int shm_fd = shm_open(shm_name, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if(shm_fd < 0)
        {
            perror(NULL);
            return 0;
        }
        size_t shm_size = page_size * nrcuv;
        if(ftruncate(shm_fd, shm_size) == -1)
        {
            perror(NULL);
            shm_unlink(shm_name);
            return 0;
        }

        char *shm_ptr;
        for(int i = 0; i < nrcuv; ++i)
        {
            shm_ptr = mmap(0, page_size, PROT_WRITE, MAP_SHARED, shm_fd, page_size*i);
            if(shm_ptr == MAP_FAILED)
            {
                perror(NULL);
                shm_unlink(shm_name);
                return errno;
            }

            int len_cuv = 0;
            while((c = fgetc(fin)) != EOF)
            {
				if(c == ' ') break;
				else
                {
                    shm_ptr += sprintf(shm_ptr, "%c", c);
                    len_cuv++;
                }
            }

            for(int j = 0; j < len_cuv; ++j)  fscanf(finPerms, "%d", &perms[i][j]);
        }

        for(int i = 0; i < nrcuv; ++i)
		{
			shm_ptr = mmap(0, page_size, PROT_READ, MAP_SHARED, shm_fd, page_size*i);
			pid_t pid = fork();
			if(pid < 0) return errno;
			else if(pid == 0)
			{
				int len_cuv = strlen(shm_ptr);

				char decripted[len_cuv+1];
				for(int j = 0; j < len_cuv; ++j) decripted[perms[i][j]] = shm_ptr[j];
				
				for(int j = 0; j < len_cuv; ++j) fprintf(fout, "%c", decripted[j]);
				if(i != nrcuv-1) fprintf(fout, " ");
                exit(0);
			}
            else wait(NULL);
			munmap(shm_ptr, page_size);
		}

        printf("Fisierul a fost decriptat cu succes!\n");
        shm_unlink(shm_name);
        fclose(fin);
        fclose(finPerms);
        fclose(fout);
    }
    else printf("Criptare: ./encriptor [nume_fisier]\nDecriptare: ./encriptor [nume_fisier] [nume_fisier_permutari]\n");
    return 0;
}
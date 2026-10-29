#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define ARQUIVO "compartilhado.dat"
#define TAM_TEXTO 256

struct Compartilhado {
    int estado;
    char texto[TAM_TEXTO];
};

int main (void)
{
    volatile struct Compartilhado *mem;
    struct stat sb;
    int fd, i, fim;

    while (access (ARQUIVO, F_OK) == -1)
        sleep (1);

    fd = open (ARQUIVO, O_RDWR);
    if (fd == -1) { perror ("open"); exit(1); }

    if (fstat (fd, &sb) == -1) { perror ("fstat"); exit(1); }
    if (sb.st_size < sizeof(struct Compartilhado)) {
        fprintf (stderr, "%s ainda nao tem o tamanho esperado\n", ARQUIVO);
        exit(1);
    }

    mem = mmap (0, sizeof(struct Compartilhado), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) { perror ("mmap"); exit(1); }
    if (close (fd) == -1) { perror ("close"); exit(1); }

    printf ("prog2: arquivo %s mapeado. Aguardando mensagens...\n", ARQUIVO);
    fflush (stdout);

    fim = 0;
    while (!fim) {
        while (mem->estado != 1)
            sleep (1);

        printf ("prog2 recebeu:  %s\n", (char *) mem->texto);

        if (mem->texto[0] == 'f' && mem->texto[1] == 'i' &&
            mem->texto[2] == 'm' && mem->texto[3] == '\0')
            fim = 1;

        for (i = 0; mem->texto[i] != '\0'; i++)
            if (mem->texto[i] >= 'a' && mem->texto[i] <= 'z')
                mem->texto[i] = mem->texto[i] - 'a' + 'A';

        printf ("prog2 respondeu: %s\n", (char *) mem->texto);
        fflush (stdout);

        mem->estado = 2;
    }

    if (munmap ((void *) mem, sizeof(struct Compartilhado)) == -1) { perror ("munmap"); exit(1); }
    printf ("prog2: fim\n");
    return 0;
}
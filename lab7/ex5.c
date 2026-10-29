#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main (int argc, char *argv[])
{
    struct stat sb;
    char *p;
    char novo[] = "Texto acrescentado pela memoria mapeada!\n";
    char zeros[sizeof(novo)] = { 0 };
    off_t tam_antigo, tam_novo, i;
    int fd;

    if (argc < 2) { fprintf (stderr, "usage: %s <file>\n", argv[0]); exit(1); }

    fd = open (argv[1], O_RDWR | O_APPEND);
    if (fd == -1) { perror ("open"); exit(1); }

    if (fstat (fd, &sb) == -1) { perror ("fstat"); exit(1); }
    if (!S_ISREG (sb.st_mode)) { fprintf (stderr, "%s is not a file\n", argv[1]); exit(1); }

    tam_antigo = sb.st_size;
    tam_novo = tam_antigo + strlen (novo);

    p = mmap (0, tam_antigo, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) { perror ("mmap"); exit(1); }

    if (write (fd, zeros, strlen (novo)) != (ssize_t) strlen (novo)) { perror ("write"); exit(1); }

    if (munmap (p, tam_antigo) == -1) { perror ("munmap"); exit(1); }
    p = mmap (0, tam_novo, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) { perror ("mmap"); exit(1); }

    if (close (fd) == -1) { perror ("close"); exit(1); }

    for (i = 0; i < (off_t) strlen (novo); i++)
        p[tam_antigo + i] = novo[i];

    if (munmap (p, tam_novo) == -1) { perror ("munmap"); exit(1); }
    return 0;
}
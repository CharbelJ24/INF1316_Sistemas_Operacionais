#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
 
#define SAIDA_WRITE "saida_write.txt"
 
/* tempo decorrido entre inicio e fim, em microssegundos */
long tempo_us(struct timeval inicio, struct timeval fim)
{
    return (fim.tv_sec - inicio.tv_sec) * 1000000L + (fim.tv_usec - inicio.tv_usec);
}
 
int main (int argc, char *argv[])
{
    struct stat sb;
    struct timeval inicio, fim;
    off_t len;
    long pagesize, paginas, sobra;
    long t_mmap, t_write;
    char *p;
    char c;
    int fd, fdw;
 
    if (argc < 2) { fprintf (stderr, "usage: %s <file>\n", argv[0]); exit(1); }
 
    fd = open (argv[1], O_RDWR);
    if (fd == -1) { perror ("open"); exit(1); }
 
    if (fstat (fd, &sb) == -1) { perror ("fstat"); exit(1); }
 
    if (!S_ISREG (sb.st_mode)) { fprintf (stderr, "%s is not a file\n", argv[1]); exit(1); }
 
    /* ---------- a) quantas paginas o arquivo precisou ---------- */
    pagesize = sysconf (_SC_PAGESIZE);
    paginas = (sb.st_size + pagesize - 1) / pagesize;   /* divisao arredondada para cima */
    sobra = paginas * pagesize - sb.st_size;
 
    printf ("Tamanho do arquivo:            %ld bytes\n", (long) sb.st_size);
    printf ("Tamanho da pagina:             %ld bytes\n", pagesize);
    printf ("Paginas necessarias:           %ld\n", paginas);
    printf ("Memoria mapeada:               %ld bytes\n", paginas * pagesize);
    printf ("Bytes nao usados (ultima pag): %ld\n\n", sobra);
 
    p = mmap (0, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) { perror ("mmap"); exit(1); }
 
    if (close (fd) == -1) { perror ("close"); exit(1); }
 
    /* ---------- b1) escrita no arquivo mapeado ---------- */
    gettimeofday (&inicio, NULL);
    for (len = 0; len < sb.st_size; len++) {
        c = p[len];
        if (c >= 'a' && c <= 'z')
            c = c - 'a' + 'A';
        p[len] = c;
    }
    gettimeofday (&fim, NULL);
    t_mmap = tempo_us (inicio, fim);
 
    /* ---------- b2) escrita com write(), um byte por chamada ---------- */
    fdw = open (SAIDA_WRITE, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fdw == -1) { perror ("open saida"); exit(1); }
 
    gettimeofday (&inicio, NULL);
    for (len = 0; len < sb.st_size; len++) {
        c = p[len];
        if (c >= 'a' && c <= 'z')
            c = c - 'a' + 'A';
        write (fdw, &c, 1);
    }
    gettimeofday (&fim, NULL);
    t_write = tempo_us (inicio, fim);
    close (fdw);
 
    if (munmap (p, sb.st_size) == -1) { perror ("munmap"); exit(1); }
 
    printf ("Tempo de escrita com write():  %ld us\n", t_write);
    printf ("Tempo de escrita com mmap:     %ld us\n", t_mmap);
    if (t_mmap > 0)
        printf ("mmap foi %.1f vezes mais rapido\n", (double) t_write / t_mmap);
 
    return 0;
}

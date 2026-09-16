#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd1, fd2;
    int retorno;
    char linha[256];

    if (argc < 3){
        fprintf(stderr, "uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        exit(1);
    }

    fd1 = open(argv[1], O_RDONLY);
    if (fd1 == -1){
        perror("Error open() entrada");
        exit(-1);
    }

    fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd2 == -1){
        perror("Error open() saida");
        exit(-1);
    }

    close(0);
    if ((retorno = dup(fd1)) == -1){
        perror("Error dup()");
        exit(-2);
    }
    close(fd1);

    if ((retorno = dup2(fd2, 1)) == -1){
        perror("Error dup2()");
        exit(-3);
    }
    close(fd2);

    while (fgets(linha, sizeof(linha), stdin) != NULL) {
        fputs(linha, stdout);
    }
 
    return 0;
}
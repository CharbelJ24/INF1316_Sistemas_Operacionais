#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define NOME "meu_fifo"

int main(void){
    int fd;
    char linha[256];

    if (access(NOME, F_OK) == -1){
        if(mkfifo(NOME, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) != 0) {
            perror("mkfifo");
            exit(1);
        }
    }

    printf("aguardando leitor\n");
    fflush(stdout);

    fd = open(NOME, O_WRONLY);
    if (fd < 0){
        perror("open (escrita)");
        exit(1);
    }

    printf("Digite mensagens (Ctrl+D para sair):\n");
    fflush(stdout);

    while(fgets(linha, sizeof(linha), stdin) != NULL){
        write(fd, linha, strlen(linha));
    }

    printf("acabou.\n");

    close(fd);
    return 0;
}
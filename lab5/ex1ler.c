#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define NOME "meu_fifo"

int main(void){
    int fd;
    char buf[256];
    ssize_t n;

    if (access(NOME, F_OK) < 0){
        if(mkfifo(NOME, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) != 0){
            perror("mkfifo");
            exit(1);
        }
    }

    printf("aguardando escritor\n");
    fflush(stdout);

    while(1){
        fd = open(NOME, O_RDONLY);
        if(fd < 0){
            perror("open(leitura)");
            exit(1);
        }

        printf("lendo dados\n");
        fflush(stdout);

        while((n = read(fd, buf, sizeof(buf) - 1)) > 0){
            buf[n] = '\0';
            fputs(buf, stdout);
            fflush(stdout);
        }

        printf("aguardando proximo escritor\n");
        fflush(stdout);

        close(fd);
    }

    return 0;
}

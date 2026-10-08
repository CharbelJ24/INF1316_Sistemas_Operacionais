#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define NOME "ex2_fifo"

int main(void){
    int fd;
    pid_t pid1, pid2;
    char mensagem[50];
    char buf[256];
    char ch;

    if (access(NOME, F_OK) < 0){
        if(mkfifo(NOME, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) != 0){
            perror("mkfifo");
            exit(1);
        }
    }

    pid1 = fork();
    if (pid1 < 0){
        perror("fork");
        exit(1);
    }

    if (pid1 == 0){
        /*processo 1*/
        fd = open(NOME, O_WRONLY);
        if (fd < 0){
            perror("open (escrita)");
            exit(1);
        }

        strcpy(mensagem, "mensagem do processo 1 para o fifo\n");
        write(fd, mensagem, strlen(mensagem));

        close(fd);
        exit(0);
    }

    pid2 = fork();
    if (pid2 < 0){
        perror("fork");
        exit(1);
    }

    if (pid2 == 0){
        /*processo 2*/
        fd = open(NOME, O_WRONLY);
        if (fd < 0){
            perror("open (escrita)");
            exit(1);
        }

        strcpy(mensagem, "mensagem do processo 2 para o fifo\n");
        write(fd, mensagem, strlen(mensagem));

        close(fd);
        exit(0);

    }
    fd = open(NOME, O_RDONLY);
    if (fd < 0) {
        perror("open (leitura)");
        exit(1);
    }

    while (read(fd, &ch, sizeof(ch)) > 0) putchar (ch);
    close(fd);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
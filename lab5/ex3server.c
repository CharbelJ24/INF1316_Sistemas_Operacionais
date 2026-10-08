#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
 
#define FIFO_REQ  "fifo_requisicoes"
#define FIFO_RESP "fifo_respostas"
 
int main(void) {
    int fd;
    char ch;
    char buf[256];
    int i;
 
    if (access(FIFO_REQ, F_OK) == -1) {
        if (mkfifo(FIFO_REQ, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) != 0) {
            puts("Erro na criação da FIFO de requisicoes");
            return -1;
        }
    }
 
    if (access(FIFO_RESP, F_OK) == -1) {
        if (mkfifo(FIFO_RESP, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) != 0) {
            puts("Erro na criação da FIFO de respostas");
            return -1;
        }
    }

    while(1){
        if ((fd = open(FIFO_REQ, O_RDONLY)) < 0) {
            puts("Erro ao abrir a FIFO de requisicoes");
            return -2;
        }

        i = 0;
        while (read(fd, &ch, sizeof(ch)) > 0 && i < sizeof(buf) - 1) {
            buf[i] = ch;
            i++;
        }
        buf[i] = '\0';

        close(fd);

        printf("Recebido: %s", buf);

        for (i = 0; buf[i] != '\0'; i++) {
            if (buf[i] >= 'a' && buf[i] <= 'z')
                buf[i] = buf[i] - 'a' + 'A';
        }
        

        if ((fd = open(FIFO_RESP, O_WRONLY)) < 0) {
            puts("Erro ao abrir a FIFO de respostas");
            return -3;
        }

        write(fd, buf, strlen(buf));
        close(fd);
 
        printf("Respondido: %s", buf);
    }
 
    return 0;
    }

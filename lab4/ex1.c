#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char *argv[]){
    int fd[2];
    pid_t pid;
    int status;
    const char textoTX[] = "mensagem";
    char textoRX[sizeof textoTX];

    if (pipe(fd) < 0){
        puts("Erro ao abrir os pipes");
        exit(-1);
    }

    pid = fork();
    if (pid < 0){
        perror("fork");
        exit(-1);
    }
    if (pid == 0){
        close(fd[0]);
        write(fd[1], textoTX, strlen(textoTX) + 1);
        printf("Foi escrito: %s\n", textoTX);
        close(fd[1]);
        exit(0);
    }
    else{
        close(fd[1]);
        read(fd[0], textoRX, sizeof(textoRX));
        printf("Foi lido: %s\n", textoRX);
        close(fd[0]);
        waitpid(pid, &status, 0);
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void){
    int fd[2];
    pid_t pid1, pid2;

    if (pipe(fd) < 0) {
        puts("Erro ao abrir os pipes");
        exit(-1);
    }
 
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(1);
    }

    if (pid1 == 0){
        close(fd[0]);

        close(1);
        dup(fd[1]);

        close(fd[1]);

        execlp("ps", "ps", NULL);

        perror("execpl ps");
        exit(1);
    }

    pid2 = fork();
    if (pid2 < 0){
        perror("fork");
        exit(1);
    }
    if(pid2 == 0){
        close(fd[1]);

        close(0);
        dup(fd[0]);

        close(fd[0]);

        execlp("wc", "wc", NULL);

        perror("execlp wc");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
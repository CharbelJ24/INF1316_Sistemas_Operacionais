#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define TEMPO_ESCRITOR 1
#define TEMPO_LEITOR 2
#define NUM_MENSAGENS 8

void rotina_leitor(int fd_leitura, const char *nome){
    char buf[64];
    ssize_t n;

    while((n = read(fd_leitura, buf, sizeof(buf))) > 0) {
        printf("[%s] consumiu: %s\n", nome, buf);
        fflush(stdout);
        sleep(TEMPO_LEITOR);
    }

    printf("[%s] pipe fechado, encerrando.\n", nome);
    fflush(stdout);
}

int main(void){
    int fd[2];
    pid_t pid_escritor, pid_leitor1, pid_leitor2;

    if(pipe(fd) < 0){
        puts("Erro ao abrir os pipes");
        exit(-1);
    }

    pid_escritor = fork();
    if (pid_escritor < 0) {
        perror("fork escritor");
        exit(1);
    }
    if(pid_escritor == 0){
        close(fd[0]);

        int i;
        char msg[64];
        for (i = 1; i <= NUM_MENSAGENS; i++){
            snprintf(msg, sizeof(msg), "mensagem %d", i);
            write(fd[1], msg, strlen(msg) + 1);
            printf("escritor produziu: %s\n", msg);
            sleep(TEMPO_ESCRITOR);
        }

        close(fd[1]);
        exit(0);
    }

    pid_leitor1 = fork();
    if (pid_leitor1 < 0) {
        perror("fork leitor1");
        exit(1);
    }
    if(pid_leitor1 == 0){
        close(fd[1]);

        rotina_leitor(fd[0], "LEITOR 1");

        close(fd[0]);
        exit(0);
    }

    pid_leitor2 = fork();
    if (pid_leitor2 < 0) {
        perror("fork leitor2");
        exit(1);
    }
    if(pid_leitor2 == 0){
        close(fd[1]);

        rotina_leitor(fd[0], "LEITOR 2");

        close(fd[0]);
        exit(0);
    }
    
    close(fd[0]);
    close(fd[1]);
 
    waitpid(pid_escritor, NULL, 0);
    waitpid(pid_leitor1, NULL, 0);
    waitpid(pid_leitor2, NULL, 0);
 
    printf("pai - todos os processos terminaram.\n");
 
    return 0;

}
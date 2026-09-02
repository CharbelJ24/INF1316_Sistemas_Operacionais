#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUM_TROCAS 10
#define DURACAO 1

int main(void){
    pid_t pid1, pid2;
    int i;

    pid1 = fork();
    if (pid1 < 0) {
        perror("fork filho1");
        exit(1);
    }
    if (pid1 == 0) {
        while (1) {
            printf("[FILHO 1] rodando (pid %d)\n", getpid());
            sleep(1);
        }
    }
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork filho2");
        exit(1);
    }
    if (pid2 == 0) {
        while (1) {
            printf("[FILHO 2] rodando (pid %d)\n", getpid());
            sleep(1);
        }
    }

    kill(pid2, SIGSTOP);

    pid_t ativo = pid1;
    pid_t parado = pid2;

    for(i = 0; i < NUM_TROCAS; i++){
        printf("[PAI] troca %d/%d: ativo = %d, parado = %d\n", i + 1, NUM_TROCAS, ativo, parado);

        sleep(DURACAO);

        kill(ativo, SIGSTOP);
        kill(parado, SIGCONT);

        pid_t temp = ativo;
        ativo = parado;
        parado = temp;
    }

    printf("[PAI] Matando os filhos...\n");

    kill(pid1, SIGKILL);
    kill(pid2, SIGKILL);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    printf("[PAI] Filhos mortos!!!");

    return 0;
}
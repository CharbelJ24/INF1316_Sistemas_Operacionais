#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define NUM_PROCESSOS 3
#define NUM_RODADAS 3

int main(void) {
    pid_t pids[NUM_PROCESSOS];
    int quantum[NUM_PROCESSOS] = {1, 2, 2};
    char *programas[NUM_PROCESSOS] = {"./ex6", "./ex5", "./ex4"};
    int i, rodada;

    for (i = 0; i < NUM_PROCESSOS; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }

        if (pids[i] == 0) {
            execl(programas[i], programas[i], NULL);

            perror("execl");
            exit(1);
        }

        kill(pids[i], SIGSTOP);
    }

    printf("[ESCALONADOR] 3 processos criados: pid1=%d, pid2=%d, pid3=%d\n",
           pids[0], pids[1], pids[2]);
    fflush(stdout);

    for (rodada = 0; rodada < NUM_RODADAS; rodada++) {
        for (i = 0; i < NUM_PROCESSOS; i++) {
            printf("[ESCALONADOR] Rodada %d: ativando processo %d (pid %d) por %d segundo(s)\n",
                   rodada + 1, i + 1, pids[i], quantum[i]);
            fflush(stdout);

            kill(pids[i], SIGCONT);
            sleep(quantum[i]);
            kill(pids[i], SIGSTOP);
        }
    }

    printf("[ESCALONADOR] %d rodadas completas. Matando os processos...\n", NUM_RODADAS);
    fflush(stdout);

    for (i = 0; i < NUM_PROCESSOS; i++)
        kill(pids[i], SIGKILL);

    for (i = 0; i < NUM_PROCESSOS; i++)
        waitpid(pids[i], NULL, 0);

    printf("[ESCALONADOR] fim.\n");

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
 
void handler_sigkill(int sig);

int main(void) {
    void (*resultado)(int) = signal(SIGKILL, handler_sigkill);
 
    if (resultado == SIG_ERR) {
        perror("signal(SIGKILL)");
    } else {
        printf("signal() aceitou a chamada (retorno != SIG_ERR)...\n");
    }
 
    printf("Meu PID e %d. Enviando SIGKILL para mim mesmo...\n", getpid());
    fflush(stdout);
 
    kill(getpid(), SIGKILL);
 
    printf("Sobrevivi ao SIGKILL! Isso nunca deveria aparecer.\n");
 
    return 0;
}

void handler_sigkill(int sig) {
    printf("Recebi o sinal %d (SIGKILL) e sobrevivi!\n", sig);
}

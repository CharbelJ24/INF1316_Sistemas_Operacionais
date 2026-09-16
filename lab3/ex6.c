#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

time_t inicio_chamada;
int chamada_em_andamento = 0;

double custo_chamada(int seg){
    double custo;

    if (seg <= 60){
        custo = seg * 0.02;
    }
    else {
        custo = 60 * 0.02 + (seg - 60) * 0.01;
    }

    return custo;
}

void handlerUSR1(int sig) {
    inicio_chamada = time(NULL);
    chamada_em_andamento = 1;
    printf("Chamada iniciada.\n");
}
 
void handlerUSR2(int sig) {
    time_t fim;
    int duracao;
    double custo;
 
    if (!chamada_em_andamento) {
        printf("SIGUSR2 recebido, mas nenhuma chamada em andamento.\n");
        return;
    }
 
    fim = time(NULL);
    duracao = (int)(fim - inicio_chamada);
    custo = custo_chamada(duracao);
 
    printf("Chamada encerrada. Duracao: %d segundos. Custo: R$ %.2f\n",
           duracao, custo);
 
    chamada_em_andamento = 0;
}

int main(void){
    signal(SIGUSR1, handlerUSR1);
    signal(SIGUSR2, handlerUSR2);

    printf("Iniciado. PID = %d\n", getpid());
    printf("SIGUSR1 inicia e SIGUSR2 termina chamada.\n");

    while(1){
        pause();
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void handler_fpe(int sig) {
    printf("Erro: divisao por zero detectada (sinal %d - SIGFPE)!\n", sig);
    exit(1);
}
 
int main(void) {
    int a, b;
 
    signal(SIGFPE, handler_fpe);
 
    printf("Digite o primeiro numero: ");
    scanf("%d", &a);
    printf("Digite o segundo numero: ");
    scanf("%d", &b);
 
    printf("Soma: %d\n", a + b);
    printf("Subtracao: %d\n", a - b);
    printf("Multiplicacao: %d\n", a * b);
    printf("Divisao: %d\n", a / b);
 
    printf("Programa terminou normalmente.\n");
 
    return 0;
}



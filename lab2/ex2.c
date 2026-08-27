#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#define CHAVE 8752
#define TAM_MSG 256

int main (int argc, char *argv[]){
    int id;
    char *p;
    char buffer[TAM_MSG];

    printf("Digite a mensagem do dia: ");

    if (fgets(buffer, TAM_MSG, stdin) == NULL){
        fprintf(stderr, "Erro ao ler a mensagem.\n");
        exit(1);
    }

    id = shmget(CHAVE, TAM_MSG, IPC_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (id == -1){
        perror("shmget");
        exit(1);
    }

    p = (char *)shmat(id, 0, 0);
    if (p == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    strncpy(p, buffer, TAM_MSG - 1);
    p[TAM_MSG - 1] = '\0';

    printf("Mensagem gravada na memoria compartilhada (chave %d, id %d).\n", CHAVE, id);

    shmdt(p);

    return 0;
}
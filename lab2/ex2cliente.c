#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define CHAVE 8752
#define TAM_MSG 256

int main(int argc, char *argv[]) {
    int id;
    char *p;

    id = shmget(CHAVE, TAM_MSG, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (id == -1){
        perror("shmget");
        fprintf(stderr, "Nao foi possivel encontrar a memoria compartilhada (chave %d).\n", CHAVE);
        exit(1);
    }

    p = (char *)shmat(id, 0, 0);
    if (p == (void *) -1){
        perror("shmat");
        exit(1);
    }

    printf("Mensagem do dia: %s\n", p);

    if(shmdt(p) == -1){
        perror("shmdt");
        exit(1);
    }

    return 0;
}
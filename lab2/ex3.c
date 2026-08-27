#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#define TAM 20
#define N 4
#define CHAVE 42

int main(int argc, char* argv[]){
    int segV, segR, *vetor, *resul, id, pid, status;
    int k, i;
    int fatia;

    segV = shmget(IPC_PRIVATE, sizeof (int) * TAM, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    segR = shmget(IPC_PRIVATE, sizeof (int) * TAM, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    vetor = (int *)shmat(segV, 0, 0);
    resul = (int *)shmat(segR, 0, 0);

    int valores[TAM] = {42, 64, 123, 1, 9, 6, 42, 97, 43, 85, 17, 60, 12, 83, 81, 94, 60, 57, 42, 81};

    for (i = 0; i < TAM; i++){
        vetor[i] = valores[i];
    }

    for (i = 0; i < N; i++){
        resul[i] = -1;
    }

    fatia = TAM / N;
    
    for (k = 0; k < N; k++){
        if((id = fork()) < 0){
            puts("Erro na criação do novo processo");
            exit(-2);
        }
        else if (id == 0){
            int inicio = k * fatia;
            int fim = inicio + fatia;
            for (i = inicio; i < fim; i++){
                if (vetor[i] == CHAVE){
                    resul[k] = i;
                    break;
                }
            }

            printf("Filho %d verificou posicoes %d a %d. Achou: %d\n", k, inicio, fim, resul[k]);

            exit(0);
        }
    }

    for (i = 0; i < N; i++){
        pid = wait(&status);
    }

    printf("\n");

    int achou = 0;
    for (k = 0; k < N; k++){
        if (resul[k] != -1){
            printf("Chave %d encontrada na posicao %d no processo %d\n", CHAVE, resul[k], k);
            achou = 1;
        }
        else{
            printf("Chave %d não foi encontrada no processo %d\n", CHAVE, k);
        }
    }
    if (!achou){
        printf("Chave %d nao encontrada no vetor.\n", CHAVE);
    }
    shmdt (vetor);
    shmdt (resul);
 

    shmctl (segV, IPC_RMID, 0);
    shmctl (segR, IPC_RMID, 0);

    return 0;
}
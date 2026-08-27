#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

int main (int argc, char *argv[]){
    int m1, m2, mRes, *p1, *p2, *pRes, status, i, j;
    pid_t pid;
    m1 = shmget(IPC_PRIVATE, sizeof(int) * 9, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    m2 = shmget(IPC_PRIVATE, sizeof(int) * 9, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    mRes = shmget(IPC_PRIVATE, sizeof(int) * 9, IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    p1 = (int *)shmat(m1, 0, 0);
    p2 = (int *)shmat(m2, 0, 0);
    pRes = (int *)shmat(mRes, 0, 0);
    
    int valoresA[9] = {5, 7, 9, 6, 3, 6, 3, 1, 2};
    int valoresB[9] = {5, 3, 0, 6, 2, 6, 5, 7, 0};

    for (i = 0; i < 9; i++) {
        p1[i] = valoresA[i];
        p2[i] = valoresB[i];
        pRes[i] = 0;
    }

    for (i = 0; i < 3; i++) {
        pid = fork();
        if (pid < 0){
            perror("fork");
            exit(1);
        }

        if (pid == 0){
            for (j = 0; j < 3; j++){
                pRes[i * 3 + j] = p1[i * 3 + j] + p2[i * 3 + j];
            }

            exit(0);
        }
    }

    for (i = 0; i < 3; i++){
        waitpid(-1, &status, 0);
    }

    printf("Resultado:\n");
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++)
            printf("%d ", pRes[i * 3 + j]);
        printf("\n");
    }

    shmdt(p1);
    shmdt(p2);
    shmdt(pRes);

    shmctl(m1, IPC_RMID, NULL);
    shmctl(m2, IPC_RMID, NULL);
    shmctl(mRes, IPC_RMID, NULL);

    return 0;
}
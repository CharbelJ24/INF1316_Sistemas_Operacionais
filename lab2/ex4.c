#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int valor;  /* ultimo valor gerado pelo filho */
    int seq;    /* numero de sequencia, incrementado a cada novo valor */
} Area;


int main(int argc, char* argv[]){
    int seg1, seg2;
    Area *m1, *m2;
    int status;
    pid_t pid1, pid2;
    char str_seg1[16], str_seg2[16];

    seg1 = shmget(IPC_PRIVATE, sizeof (Area), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    seg2 = shmget(IPC_PRIVATE, sizeof (Area), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);

    m1 = (Area *)shmat(seg1, 0, 0);
    m2 = (Area *)shmat(seg2, 0, 0);

    m1->valor = 0;
    m1->seq = 0;
    m2->valor = 0;
    m2->seq = 0;

    sprintf(str_seg1, "%d", seg1);
    sprintf(str_seg2, "%d", seg2);

    pid1 = fork();

    if (pid1 < 0){
        perror("fork p1");
        exit(1);
    }
    else if (pid1 == 0){
        execl("./ex4filhos", "filho", str_seg1, "P1", NULL);

        perror("execl p1");
        exit(1);
    }

    pid2 = fork();

    if (pid2 < 0){
        perror("fork p2");
        exit(1);
    }
    else if (pid2 == 0){
        execl("./ex4filhos", "filho", str_seg2, "P2", NULL);

        perror("execl p2");
        exit(1);
    }

    int ult_seq1 = 0, ult_seq2 = 0;
    int produtos_impressos = 0;

    while(produtos_impressos < 3){
        if(m1->seq > ult_seq1 && m2->seq > ult_seq2){
            printf("[PAI] produto disponivel: %d * %d = %d\n",
                    m1->valor, m2->valor, m1->valor * m2->valor);

            ult_seq1 = m1->seq;
            ult_seq2 = m2->seq;
            produtos_impressos++;
        }

        usleep(1000);
    }

    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
 
    printf("[PAI] fim. %d produtos impressos.\n", produtos_impressos);
 
    shmdt(m1);
    shmdt(m2);
    shmctl(seg1, IPC_RMID, 0);
    shmctl(seg2, IPC_RMID, 0);

    return 0;
}

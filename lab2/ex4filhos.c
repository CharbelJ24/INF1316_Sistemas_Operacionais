#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

typedef struct {
    int valor;
    int seq;
} Area;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "uso: %s <id_segmento> <nome>\n", argv[0]);
        exit(1);
    }

    int id = atoi(argv[1]);
    const char *nome = argv[2];

    Area *area = (Area *)shmat(id, 0, 0);
    if (area == (void *) -1){
        perror("shmat");
        exit(1);
    }

    srand(getpid());

    for (int i = 0; i < 3; i++){
        int tempo = (rand() % 2) + 1;
        sleep(tempo);

        int valor = (rand() % 20) + 1;

        area->valor = valor;
        area->seq = area->seq + 1;

        printf("[%s] gerou valor %d (seq %d) apos dormir %ds\n",
            nome, valor, area->seq, tempo);
    }

    shmdt(area);

    return 0;
}
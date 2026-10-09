#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAXFILA 8
#define TOTAL   64

#ifndef UNIDADE_US
#define UNIDADE_US 1000000
#endif
#define TEMPO_PRODUTOR   (1 * UNIDADE_US)
#define TEMPO_CONSUMIDOR (2 * UNIDADE_US)

static int fila[MAXFILA];
static int inicio = 0;
static int fim = 0;
static int qtd = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  pode_produzir = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  pode_consumir = PTHREAD_COND_INITIALIZER;

void *produtor(void *arg)
{
    unsigned int semente = (unsigned int) time(NULL);

    for (int i = 1; i <= TOTAL; i++) {
        usleep(TEMPO_PRODUTOR);
        int valor = rand_r(&semente) % 1000;

        pthread_mutex_lock(&mutex);
        while (qtd == MAXFILA) {
            printf("fila cheia (%d/%d), esperando...\n", qtd, MAXFILA);
            pthread_cond_wait(&pode_produzir, &mutex);
        }

        fila[fim] = valor;
        fim = (fim + 1) % MAXFILA;
        qtd++;
        printf("Produziu item %2d (valor %3d)  | fila: %d/%d\n", i, valor, qtd, MAXFILA);

        pthread_cond_signal(&pode_consumir);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumidor(void *arg)
{
    for (int i = 1; i <= TOTAL; i++) {
        usleep(TEMPO_CONSUMIDOR);

        pthread_mutex_lock(&mutex);
        while (qtd == 0) { 
            printf("fila vazia, esperando...\n");
            pthread_cond_wait(&pode_consumir, &mutex);
        }

        int valor = fila[inicio];
        inicio = (inicio + 1) % MAXFILA;
        qtd--;
        printf("Consumiu item %2d (valor %3d) | fila: %d/%d\n",
               i, valor, qtd, MAXFILA);

        pthread_cond_signal(&pode_produzir);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void)
{
    pthread_t tprod, tcons;

    printf("Iniciando: MAXFILA=%d, TOTAL=%d\n\n", MAXFILA, TOTAL);

    if (pthread_create(&tprod, NULL, produtor, NULL) != 0 ||
        pthread_create(&tcons, NULL, consumidor, NULL) != 0) {
        fprintf(stderr, "Erro ao criar threads\n");
        return 1;
    }

    pthread_join(tprod, NULL);
    pthread_join(tcons, NULL);

    printf("\nFim: %d itens produzidos e consumidos. Itens restantes na fila: %d\n", TOTAL, qtd);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&pode_produzir);
    pthread_cond_destroy(&pode_consumir);
    return 0;
}
/*
 * Exercício 4 - Produtor/Consumidor com VÁRIOS produtores e consumidores
 *
 * - NPROD threads produtoras: cada uma gera um inteiro a cada 1 s
 * - NCONS threads consumidoras: cada uma retira um inteiro a cada 2 s
 * - Fila circular com no máximo MAXFILA = 8 elementos
 * - Ao todo são produzidos e consumidos TOTAL = 64 elementos,
 *   divididos entre as threads
 *
 * Compilar: gcc -Wall -o prod_cons_multi prod_cons_multi.c -lpthread
 * Teste rápido (50 ms no lugar de 1 s): acrescente -DUNIDADE_US=50000
 * Outras quantidades de threads: -DNPROD=3 -DNCONS=4, por exemplo
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAXFILA 8
#define TOTAL   64

#ifndef NPROD
#define NPROD 2
#endif
#ifndef NCONS
#define NCONS 2
#endif
#ifndef UNIDADE_US
#define UNIDADE_US 1000000
#endif
#define TEMPO_PRODUTOR   (1 * UNIDADE_US)
#define TEMPO_CONSUMIDOR (2 * UNIDADE_US)

typedef struct {
    int valor;
    int produtor;
    int seq;
} Item;

static Item fila[MAXFILA];
static int inicio = 0, fim = 0, qtd = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  pode_produzir = PTHREAD_COND_INITIALIZER;
static pthread_cond_t  pode_consumir = PTHREAD_COND_INITIALIZER;

static int cota(int id, int n) { return TOTAL / n + (id < TOTAL % n ? 1 : 0); }

void *produtor(void *arg)
{
    int id = (int)(intptr_t) arg;
    unsigned int semente = (unsigned int) time(NULL) + id * 7919;
    int n = cota(id, NPROD);

    for (int i = 1; i <= n; i++) {
        usleep(TEMPO_PRODUTOR);
        Item it = { rand_r(&semente) % 1000, id, i };

        pthread_mutex_lock(&mutex);
        while (qtd == MAXFILA) {
            printf("   [P%d] fila cheia, esperando\n", id);
            pthread_cond_wait(&pode_produzir, &mutex);
        }
        fila[fim] = it;
        fim = (fim + 1) % MAXFILA;
        qtd++;
        printf("P%d produziu %d#%-2d (valor %3d) | fila %d/%d\n",
               id, id, i, it.valor, qtd, MAXFILA);
        pthread_cond_signal(&pode_consumir);
        pthread_mutex_unlock(&mutex);
    }
    printf("   [P%d] terminou (%d itens)\n", id, n);
    return NULL;
}

void *consumidor(void *arg)
{
    int id = (int)(intptr_t) arg;
    int n = cota(id, NCONS);

    for (int i = 1; i <= n; i++) {
        usleep(TEMPO_CONSUMIDOR);

        pthread_mutex_lock(&mutex);
        while (qtd == 0) {
            printf("   [C%d] fila vazia, esperando\n", id);
            pthread_cond_wait(&pode_consumir, &mutex);
        }
        Item it = fila[inicio];
        inicio = (inicio + 1) % MAXFILA;
        qtd--;
        printf("                                    C%d consumiu %d#%-2d (valor %3d) | fila %d/%d\n",
               id, it.produtor, it.seq, it.valor, qtd, MAXFILA);
        pthread_cond_signal(&pode_produzir);
        pthread_mutex_unlock(&mutex);
    }
    printf("   [C%d] terminou (%d itens)\n", id, n);
    return NULL;
}

int main(void)
{
    pthread_t tp[NPROD], tc[NCONS];
    intptr_t i;

    printf("Iniciando: %d produtores, %d consumidores, MAXFILA=%d, TOTAL=%d\n\n",
           NPROD, NCONS, MAXFILA, TOTAL);

    for (i = 0; i < NPROD; i++) pthread_create(&tp[i], NULL, produtor, (void *) i);
    for (i = 0; i < NCONS; i++) pthread_create(&tc[i], NULL, consumidor, (void *) i);

    for (i = 0; i < NPROD; i++) pthread_join(tp[i], NULL);
    for (i = 0; i < NCONS; i++) pthread_join(tc[i], NULL);

    printf("\nFim: %d itens. Itens restantes na fila: %d\n", TOTAL, qtd);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&pode_produzir);
    pthread_cond_destroy(&pode_consumir);
    return 0;
}
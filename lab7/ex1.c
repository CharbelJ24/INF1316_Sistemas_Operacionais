#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

void *memorypool;

void *myallocate( int n){
    return sbrk(n);
}
void initmemorypool(void) {
    memorypool = sbrk(0);
}
void resetmemorypool(void){
    brk(memorypool);
}

int main(void) {
    int * vetor;
    int i;

    initmemorypool();
    printf("Endereço do limite do heap: %p\n", memorypool);

    vetor = (int *)myallocate(10 * sizeof(int));
    if(vetor == (void *) -1){
        printf("Erro ao alocar vetor\n");
        return -1;
    }
    for (i = 0; i < 10; i++)
        vetor[i] = i;

    printf("\nvetor alocado em %p\n", vetor);
    printf("Limite do heap agora: %p\n", sbrk(0));
    printf("Conteudo do vetor: ");
    for (i = 0; i < 10; i++)
        printf("%d ", vetor[i]);
    printf("\n");

    resetmemorypool();

    return 0;
}
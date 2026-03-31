#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<semaphore.h>
typedef struct {
    int *buffer;
    int size;
    int head;
    int tail;
    int M;
    pthread_mutex_t mutex;
    sem_t empty;
    sem_t full;
} SharedData;
void put_item(SharedData *data, int item) {
    pthread_mutex_lock(&data->mutex);
    data->buffer[data->head] = item;
    data->head = (data->head + 1) % data->size;
    pthread_mutex_unlock(&data->mutex);
}
int get_item(SharedData *data) {
    int item; 
    pthread_mutex_lock(&data->mutex);
    item = data->buffer[data->tail];
    data->tail = (data->tail + 1) % data->size;
    pthread_mutex_unlock(&data->mutex);
    return item; 
}
void *producer(void *arg){
    SharedData *data = (SharedData *)arg;
    int count = data->M;
    for(int i=0;i<count;i++){
        int item = rand() % 100;
        sem_wait(&data->empty);
        put_item(data, item);
        sem_post(&data->full);
        printf("Wyprodukowano: %d\n", item);
    }
    return NULL;
}
void *consumer(void *arg){
    SharedData *data = (SharedData *)arg;
    int count = data->M;
    for(int i=0;i<count;i++){
        sem_wait(&data->full);
        int item = get_item(data);
        sem_post(&data->empty);
        printf("Skonsumowano: %d\n", item);
    }
    return NULL;
}
int main(int argc, char *argv[]){
    if(argc!=3){
        printf("Blad ilosci danych\n");
        return 1;
    }
    int M=atoi(argv[1]);
    int N=atoi(argv[2]);
    SharedData data;
    data.size=N;
    data.M=M;
    data.tail=0;
    data.head=0;
    data.buffer=malloc(N*sizeof(int));
    pthread_mutex_init(&data.mutex,NULL);
    sem_init(&data.empty,0,N);
    sem_init(&data.full,0,0);

    printf("Start: M=%d, N=%d. Bufor o rozmiarze %d.\n", M, N, N);

    pthread_t prod[2], cons[2];

    pthread_create(&prod[0], NULL, producer, &data);
    pthread_create(&prod[1], NULL, producer, &data);
    
    pthread_create(&cons[0], NULL, consumer, &data);
    pthread_create(&cons[1], NULL, consumer, &data);
    pthread_join(prod[0], NULL);
    pthread_join(prod[1], NULL);
    pthread_join(cons[0], NULL);
    pthread_join(cons[1], NULL);

    free(data.buffer);
    pthread_mutex_destroy(&data.mutex);
    sem_destroy(&data.empty);
    sem_destroy(&data.full);

    return 0;
}

#include "zad2.h"
int main()
{
    int shm_fd=shm_open(SHM_NAME,O_CREAT | O_RDWR,0666);
    if (shm_fd==-1)
    {
        printf("Blad otwarcia pamieci wspoldzielonej\n");
        return 1;
    }
    if(ftruncate(shm_fd,sizeof(struct SharedData))==-1)
    {
        printf("Blad ustalenia rozmiaru pamieci wspoldzielonej\n");
        return 1;
    }
    struct SharedData *dane =(struct SharedData*)mmap(
        NULL,
        sizeof(struct SharedData),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        shm_fd,
        0
    );
    if(dane==MAP_FAILED)
    {
        printf("Blad odwzorowania pamieci wspoldzielonej\n");
        return 1;   
    }
    dane->head=0;
    dane->tail=0;
    printf("Pamiec zmapowana pod adresem %p\n",dane);
    sem_t *sem_empty=sem_open(SEM_EMPTY,O_CREAT,0666,BUF_SIZE);
    sem_t *sem_full=sem_open(SEM_FULL,O_CREAT,0666,0);
    if(sem_empty==SEM_FAILED || sem_full==SEM_FAILED)
    {
        printf("Blad tworzenia semaforow\n");
        return 1;
    }
    for(int i=0;i<10;i++)
    {
        int liczba=rand()%100;
        sem_wait(sem_empty);
        dane->bufor[dane->head]=liczba;
        printf("Wyprodukowano: %d na pozycji %d\n",liczba,dane->head);
        dane->head=(dane->head+1)%BUF_SIZE;
        sem_post(sem_full);
        sleep(1);
    }
    sem_close(sem_empty);
    sem_close(sem_full);
    munmap(dane,sizeof(struct SharedData));
    close(shm_fd);
    return 0;
}
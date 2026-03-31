#include "zad2.h"
int main()
{
    int shm_fd=shm_open(SHM_NAME,O_CREAT | O_RDWR,0666);
    if (shm_fd==-1)
    {
        printf("Blad otwarcia pamieci wspoldzielonej\n");
        return 1;
    }
    struct SharedData *dane=(struct SharedData *)mmap(
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
    sem_t *sem_empty=sem_open(SEM_EMPTY,0);
    sem_t *sem_full=sem_open(SEM_FULL,0);
    if(sem_empty==SEM_FAILED || sem_full==SEM_FAILED)
    {
        printf("Blad otwarcia semaforow\n");
        return 1;
    }
    for(int i=0;i<10;i++)
    {
        sem_wait(sem_full);
        int liczba=dane->bufor[dane->tail];
        printf("Odebrano: %d z pozycji %d\n",liczba,dane->tail);
        dane->tail=(dane->tail+1)%BUF_SIZE;
        sem_post(sem_empty);
        sleep(1);
    }
    sem_close(sem_empty);
    sem_close(sem_full);
    munmap(dane,sizeof(struct SharedData));
    close(shm_fd);
    return 0;
}
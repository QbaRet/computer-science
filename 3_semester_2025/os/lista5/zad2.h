#ifndef WSPOLNE_H
#define WSPOLNE_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

#define SHM_NAME "/pamiec"
#define SEM_EMPTY "/sem_empty"
#define SEM_FULL "/sem_full"
#define BUF_SIZE 5

struct SharedData {
    int bufor[BUF_SIZE];
    int head;
    int tail;
};

#endif
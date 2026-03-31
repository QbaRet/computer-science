#include<stdio.h>
#include<signal.h>
#include<unistd.h>
void signal_handler(int sign)
{
   printf("Sygnał %d przechwycony\n", sign); 
}
main()
{
    for (int i = 1; i < 65; i++) {
        if (signal(i, signal_handler) == SIG_ERR) {
            printf("Nie mozna przechwycic sygnalu \n");
        } else {
            signal_handler(i);
        }
    }
    printf("PODPUNKT B: \n");
    if(kill(1,SIGKILL)==0)
    {
        printf("Udalo sie zabic proces \n");
    } else {
        printf("Nie udalo sie zabic procesu \n");
    }
    
}

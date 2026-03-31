#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
volatile int licznik = 0;
void signal_handler(int sign)
{
    if (sign == SIGUSR1) {
        licznik += 1;
        sleep(5);
        printf("Odebrano sygnal: %d razy \n", licznik);
    }
}
int main()
{
    if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
        printf("Zly sygnal");
        return 1;
    }
    while (1) {
        pause(); 
    }
}
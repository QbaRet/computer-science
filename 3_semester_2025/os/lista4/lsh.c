#include<stdio.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdbool.h>
#include<fcntl.h>
void signal_handler(int sign)
{
    if(sign==SIGCHLD)
    {
        signal(SIGCHLD,signal_handler);
        while (waitpid(-1, NULL, WNOHANG) > 0);
    }
}
int main(){
    signal(SIGINT,SIG_IGN);
    signal(SIGCHLD,signal_handler);
    char prompt[100];
    while(1)
    {        
        char *in_file=NULL;
        char *out_file=NULL;
        char *err_file=NULL;
        char *args[20];
        int i=0;
        bool bg=false;
        printf("lsh> ");
        fgets(prompt,100,stdin);
        prompt[strcspn(prompt, "\n")]=0;
        char *wyraz=strtok(prompt," ");
        while(wyraz!=NULL)
        {
            args[i]=wyraz;
            wyraz=strtok(NULL," ");
            i++;
        }
        args[i]=NULL;
        if(args[0]==NULL)
        {
            continue;
         }
        if(strcmp(args[i-1],"&")==0)
        {
            args[i-1]=NULL;
            bg=true;
        }

        if(strcmp(args[0],"exit")==0)
        {
            return 0;
        }
        if(strcmp(args[0],"cd")==0)
        {
            char *path;
            if(args[1]==NULL)
            {
                path = getenv("HOME");

            }
            else{
                path=args[1];                
            }
            if(chdir(path)!=0)
            {
                printf("Blad");
            }
            continue;
        }
        int j=0;
        i=0;
        char *clean_args[20];
        while (args[i] != NULL) 
        {
            if (strcmp(args[i], "<") == 0) {
                in_file = args[i + 1];
                i += 2; 
            } 
            else if (strcmp(args[i], ">") == 0) {
                out_file = args[i + 1];
                i += 2; 
            }
            else if (strcmp(args[i], "2>") == 0) {
                err_file = args[i + 1];
                i += 2; 
            }
            else {
                clean_args[j] = args[i];
                j++;
                i++; 
            }
        }
        clean_args[j] = NULL; 
        int k;
        for (k = 0; clean_args[k] != NULL; k++) {
            args[k] = clean_args[k];
        }
        args[k] = NULL;

        int pipe_idx = -1;
        for (int p = 0; args[p] != NULL; p++) {
            if (strcmp(args[p], "|") == 0) {
                pipe_idx = p;
                break;
            }
        }
        if (pipe_idx != -1) {
            args[pipe_idx] = NULL;        
            char **args2 = &args[pipe_idx + 1]; 
            int pipefd[2];
            if (pipe(pipefd) == -1) {
                perror("lsh: Blad");
                continue;
            }
            if (fork() == 0) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
                execvp(args[0], args);
                perror("Blad pierwszej czesci potoku");
                exit(1);
            }
            if (fork() == 0) {
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
                execvp(args2[0], args2);
                perror("Blad drugiej czesci potoku");
                exit(1);
            }
            wait(NULL);
            wait(NULL);
            continue;
        }
        int pid = fork();
        if (pid==0)
        {
            signal(SIGINT,SIG_DFL);
            int fd;
            if(in_file!=NULL)
            {
                fd=open(in_file, O_RDONLY);
                if(fd==-1)
                {
                    printf("Blad otwarcia pliku");
                    exit(1);
                }
                dup2(fd,STDIN_FILENO);
                close(fd);
            }
            if(out_file!=NULL)
            {
                fd=open(out_file,O_WRONLY | O_CREAT | O_TRUNC,0644);
                if(fd==-1)
                {
                    printf("Blad otwarcia wyjscia");
                    exit(1);
                }
                dup2(fd,STDOUT_FILENO);
                close(fd);
            }
            if(err_file!=NULL)
            {
                fd=open(err_file,O_WRONLY | O_CREAT | O_TRUNC,0644);
                if(fd==-1)
                {
                    printf("Blad otwarcia bledu");
                    exit(1);
                }
                dup2(fd,STDERR_FILENO);
                close(fd);
            }
            execvp(args[0], args);
            printf("Command not found\n");
            exit(1);
        }
        else if(bg==false){
            waitpid(pid,NULL,0);
        }
    }
}

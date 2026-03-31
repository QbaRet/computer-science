#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/select.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int id;
    char login[50];
} Client;

Client clients[MAX_CLIENTS];
int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Użycie: %s <port>\n", argv[0]);
        return 1;
    }
    int port = atoi(argv[1]);
    int listed_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(listed_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(listed_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Błąd bind");
        exit(EXIT_FAILURE);
    }

    listen(listed_fd, 10);
    for(int i=0; i<MAX_CLIENTS; i++){
        clients[i].id = -1;
    }
    printf("Serwer działa na porcie %d\n", port);
    while(1){
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(listed_fd, &read_fds);
        int max_fd = listed_fd;

        for(int i=0; i<MAX_CLIENTS; i++){
            if(clients[i].id != -1){
                FD_SET(clients[i].id, &read_fds);
                if(clients[i].id > max_fd) max_fd = clients[i].id;
            }
        }

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("Błąd select");
            continue;
        }
        if(FD_ISSET(listed_fd, &read_fds)){
            int new_fd = accept(listed_fd, NULL, NULL);
            int added = 0;
            for(int i=0; i<MAX_CLIENTS; i++){
                if(clients[i].id == -1){
                    clients[i].id = new_fd;
                    clients[i].login[0] = '\0';
                    added = 1;
                    break;
                }
            }
            if (!added) close(new_fd); 
        for(int i=0; i<MAX_CLIENTS; i++){
            int sd = clients[i].id;
            if(sd > 0 && FD_ISSET(sd, &read_fds)){
                char buffer[BUFFER_SIZE];
                int valread = recv(sd, buffer, BUFFER_SIZE - 1, 0);               
                
                if(valread <= 0){
                    close(sd);
                    clients[i].id = -1;
                    clients[i].login[0] = '\0';
                } else {
                    buffer[valread] = '\0';
                    buffer[strcspn(buffer, "\r\n")] = 0;

                    if(clients[i].login[0] == '\0'){
                        strncpy(clients[i].login, buffer, sizeof(clients[i].login) - 1);
                        char list[BUFFER_SIZE] = "Zalogowani:\n";
                        for(int j=0; j<MAX_CLIENTS; j++){
                            if(clients[j].id != -1 && clients[j].login[0] != '\0'){
                                char entry[100];
                                snprintf(entry, sizeof(entry), "- %s (ID:%d)\n", clients[j].login, clients[j].id);
                                strcat(list, entry);
                            }
                        }
                        send(sd, list, strlen(list), 0);
                    } else if (strcmp(buffer, "QUIT") == 0) {
                        close(sd);
                        clients[i].id = -1;
                        clients[i].login[0] = '\0';
                    } else if (strncmp(buffer, "SEND ", 5) == 0) {
                        char *target = strtok(buffer + 5, " ");
                        char *message = strtok(NULL, "");
                        if(target && message){
                            for(int j=0; j<MAX_CLIENTS; j++){
                                if(clients[j].id != -1 && strcmp(clients[j].login, target) == 0){
                                    char out[BUFFER_SIZE];
                                    snprintf(out, sizeof(out), "OD %s: %s\n", clients[i].login, message);
                                    send(clients[j].id, out, strlen(out), 0);
                                }
                            }
                        }
                    }
                }
            }
        } 
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 9991
#define BUFFER_SIZE 1024

int sock_dist;

void *tCPMessageCallBack() {
    char buffer[BUFFER_SIZE] = {0};
    while(1) {

        int datalen = read(sock_dist, buffer, BUFFER_SIZE);


        if (datalen != 0) {
            printf("%s\n", buffer);
            

            fflush(stdout); 
            memset(buffer, 0, BUFFER_SIZE); 
            send(sock_dist, "hello", strlen("hello"), 0);
        }
    }
}



/// gcc TCPSERVER.c -o server -lpthread

int main() {
    int socket_local;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Créer le socket et initialiser l'adresse
    socket_local = socket(AF_INET, SOCK_STREAM, 0);
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associer le socket à l'adresse de l'interface
    bind(socket_local, (struct sockaddr *)&address, sizeof(address));
    
    // Attendre une connexion entrante
    listen(socket_local, 3);
    sock_dist = accept(socket_local, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    pthread_t t_tcp_message;

    if (pthread_create(&t_tcp_message,NULL,tCPMessageCallBack,'\0') != 0) {
        printf("ERROR AT TCP ONMESSAGE CALLBACK THREAD\n");
        return 1;
    }

    pthread_join(t_tcp_message, NULL);
    close(sock_dist);
    close(socket_local);
    return 0;
}
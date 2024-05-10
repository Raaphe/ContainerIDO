#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 9991
#define DEST_IP "10.10.20.254"
#define BUFFER_SIZE 1024
#define ANSWER_LEN 500

void *messageCallBackTCP(int *args) {

    while (1) {
        printf("> ");
        char message[100];
        char answer[4];

        if(strcmp(message,"exit\n") == 0){
            break;
        } else {                     
            // Afficher réponse
            recv(args[0], answer, ANSWER_LEN, 0);
            printf("< %s\n",answer);
        }
    }
    return ;
}

int main() {
    int sock = 0;
    struct sockaddr_in dest_addr;
    
    // Créer le socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Initialiser la struct de l'adresse IP 
    memset(&dest_addr, '0', sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    dest_addr.sin_port = htons(PORT);

    // Créer la connexion
    connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    // Créer les threads

    pthread_t t_received_tcp;
    int args1[]  = {sock};

    if (pthread_create(&t_received_tcp, args1,messageCallBackTCP,NULL) != 0) {
        printf("Erreur à la création du thread pour LED 1.\n");
        return 1;
    }

    // Attendre la fin de l'exécution de chaque thread
    pthread_join(t_received_tcp, NULL);
    printf("Fin du thread 1.\n");
    
    // fgets(message,sizeof(message),stdin);
    // send(sock, message, strlen(message), 0);


    close(sock);
    return 0;
}
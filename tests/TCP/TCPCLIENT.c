#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// THREADS
#include <pthread.h>

#define PORT 9991
#define DEST_IP "127.0.0.1"
#define ANSWER_LEN 100


/// gcc TCPCLIENT.c -o client -lpthread

int sock = 0;
struct sockaddr_in dest_addr;

void *tCPMessageCallBack()
{
    // Envoyer le message et fermer la connexion
    while (1) {
        printf("> ");
        char message[100];
        char answer[100];

        fgets(message,sizeof(message),stdin);
        send(sock, message, strlen(message), 0);

        if(strcmp(message,"exit\n") == 0){
            break;
        } else {                                // ** ELSE AJOUTE ** //
            // Afficher réponse
            recv(sock, answer, ANSWER_LEN, 0);
            printf("< %s\n",answer);
        }
    }
}

int main() {

    
    // Créer le socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Initialiser la struct de l'adresse IP 
    memset(&dest_addr, '0', sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    dest_addr.sin_port = htons(PORT);

    // Créer la connexion
    connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    
    pthread_t t_tcp_message;

    if (pthread_create(&t_tcp_message, NULL, tCPMessageCallBack, '\0') != 0)
    {
        printf("ERROR AT TCP ONMESSAGE CALLBACK THREAD\n");
        return 1;
    }

    pthread_join(t_tcp_message, NULL);
    return 0;
}
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// THREADS
#include <pthread.h>

// MOSQUITTO LIB
#include <mosquitto.h>

// TCP UTILS
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// gcc container.c -o test -lmosquitto -lpthread

/// <<< START CONSTANTS
#define MQTT_BROKER_HOST "10.10.0.197"
#define MQTT_TOPIC "final"
#define MQTT_PORT 8883
#define MQTT_QOS 1

#define TEAM_NUMBER 3

#define PI_IP "10.10.20.254"
#define PORT 9991
#define BUFFER_SIZE 1024
/// >>> END CONSTANTS

/// <<< START GLOBAL VARIABLES
int sock;
struct mosquitto *mosq = NULL;
/// >>> END GLOBAL VARIABLES

/// <<< START FUNCTIONS

/**
 * MQTT Message callback method used when connected to Broker.
 */
void on_connect(struct mosquitto *mosq, void *userdata, int result)
{
    if (result == 0)
    {
        mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, MQTT_QOS);
    }
    else
    {
        fprintf(stderr, "Erreur: connexion broker MQTT.\n");
    }
}

/**
 * The MQTT Callback method used when sending a message
 */
void on_publish(struct mosquitto *mosq, void *userdata, int mid)
{
    printf("State sent.\n");
}

/**
 * The MQTT Message callback method. Receives incoming messages from broker.
 * On Mqtt message, I should parse through the message of this format : <team_num>:<on|off>.
 * Once I do this I need to send my raspi counter-part a the verbatim message of this format through our TCP connection: <team_num>:<1|0>
 */
void on_message(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    printf("Getting message from mqtt\n");
    if (((char *)message->payload)[0] == TEAM_NUMBER)
    {
        return;
    }

    const char splitter[2] = ":";
    char *token;

    token = strtok(message->payload, splitter);
    char logBuffer[10];
    int first = 1;

    while (token != NULL)
    {
        printf(" %s\n", token);

        if (first)
        {
            first = 0;
            strcat(token, logBuffer);
            strcat(token, ":");
        }
        else
        {
            strcat(token, logBuffer);
        }
        token = strtok(NULL, splitter);
    }

    send(sock, logBuffer, strlen(logBuffer), 0);
    printf("Message: (%s) %s\n", message->topic, (char *)message->payload);
}

void sendTeamLogToPi(int teamNumber, int status)
{
}

void postTeamLogToTopic(int status)
{
}

/**
 * CREDIT TO THIS METHOD GOES TO [ADAM ROSENFIELD](https://stackoverflow.com/users/9530/adam-rosenfield).
 * [LINK](https://stackoverflow.com/a/122721)
 */
size_t trimwhitespace(char *out, size_t len, const char *str)
{
    if (len == 0)
        return 0;

    const char *end;
    size_t out_size;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
    {
        *out = 0;
        return 1;
    }

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;
    end++;

    // Set output size to minimum of trimmed string length and buffer size minus 1
    out_size = (end - str) < len - 1 ? (end - str) : len - 1;

    // Copy trimmed string and add null terminator
    memcpy(out, str, out_size);
    out[out_size] = 0;

    return out_size;
}

/**
 * The method triggered when I receive a message through my TCP connection.
 * Once I receive a message, it will either be 0 or 1. I then post to the MQTT topic a log of this format :
 * <my_team_num>:<received_status>.
 */
void *tCPMessageCallBack()
{
    char answer[100];
    char output[100];
    while (1) {
        ssize_t receivedBytes = recv(sock, answer, sizeof(answer) - 1, 0); // Subtract 1 for the null terminator
        if (receivedBytes == -1) {
            perror("Error receiving message");
            break; 
        } else if (receivedBytes == 0) {
            printf("Connection closed\n");
            break; 
        }

        answer[receivedBytes] = '\0'; // Null terminate the received string
        printf("< %s\n", answer);

        trimwhitespace(output, sizeof(output), answer);    

        strcat(output, ":");
        
        char str[5]; // Ensure enough space for the integer representation
        sprintf(str, "%d", TEAM_NUMBER);

        strcat(output, str);

        mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(output), output, 0, false);
    }
}
/// >>> END FUNCTIONS

/// <<< START MAIN
int main()
{

    /// <<< START MQTT
    int rc;

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq)
    {
        fprintf(stderr, "Erreur: création de l'instance mosquitto.\n");
        return 1;
    }

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);
    mosquitto_username_pw_set(mosq, "raph", "abc-123");

    char *ca = "/etc/mosquitto/ca.crt";
    char *cert = "/etc/mosquitto/raph.crt";
    char *key = "/etc/mosquitto/hote.key";
    mosquitto_tls_set(mosq, ca, NULL, cert, key, NULL);

    rc = mosquitto_connect(mosq, MQTT_BROKER_HOST, MQTT_PORT, 60);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Connexion impossible au broker: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    /// >>> END MQTT

    /// <<< START TCP
    struct sockaddr_in dest_addr;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&dest_addr, '0', sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(PI_IP);
    dest_addr.sin_port = htons(PORT);

    connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    /// >>> END TCP

    /// <<< START THREADS
    pthread_t t_tcp_message;

    if (pthread_create(&t_tcp_message, NULL, tCPMessageCallBack, '\0') != 0)
    {
        printf("ERROR AT TCP ONMESSAGE CALLBACK THREAD\n");
        return 1;
    }

    /// >>> END THREADS

    /// <<< START GARBAGE COLLECTOR
    close(sock);
    mosquitto_loop_forever(mosq, -1, 1);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    pthread_join(t_tcp_message, NULL);
    /// >>> END GARBAGE COLLECTOR

    return 0;
}
/// >>> END MAIN
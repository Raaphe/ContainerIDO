#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// MOSQUITTO LIB
#include <mosquitto.h>

// #define MQTT_BROKER_HOST "10.10.0.197"
// #define MQTT_TOPIC "final"
#define MQTT_BROKER_HOST "127.0.0.1"
#define MQTT_TOPIC "test"

#define MQTT_PORT 8883
#define MQTT_QOS 1
#define MQTT_MESSAGE "12:off"

// gcc MQTTPUB.c -o publisher -lmosquitto

/**
 * The MQTT Callback method used when sending a message
*/
void on_publish(struct mosquitto *mosq, void *userdata, int mid) {
    printf("State sent.\n");
}

int main(int argc, int **argv) {

    struct mosquitto *mosq = NULL;
    int rc, sendRc;

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Erreur: création de l'instance mosquitto.\n");
        return 1;
    }

    mosquitto_username_pw_set(mosq,"raph","abc-123");

    char* ca = "/etc/mosquitto/ca.crt";
    char* cert = "/etc/mosquitto/raph.crt";
    char* key = "/etc/mosquitto/hote.key";
    mosquitto_tls_set(mosq, ca, NULL, cert, key, NULL); 

    sendRc = mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(MQTT_MESSAGE), MQTT_MESSAGE, 0, false);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to publish message: %s\n", mosquitto_strerror(rc));
    }

    return 0;
}
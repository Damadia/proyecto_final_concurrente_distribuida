#ifndef MONITORS_H
#define MONITORS_H

#include "dependencies.h"

#define MAX_SERVERS_AUTH 3 //   Máximo de servidores que se van a crear, y por extensió para los demás archvios, máximo de hilos
#define MAX_FIREWALLS 3 //no lo terminé usando
#define CONDITION_EXIT 100
#define MAX_CLIENTS_AUTH 5
#define MAX_CLIENTS_FIRE 5

typedef struct {
    pthread_t thdsAuth[MAX_SERVERS_AUTH];
    pthread_t thdsFire[MAX_FIREWALLS];
    pthread_mutex_t mutexAuth, mutexFire;
    int socket_fd;
    int countAuth, countFire;
} mainMonitor;

typedef struct {
    pthread_t thdsAuth[MAX_CLIENTS_AUTH];
    pthread_mutex_t mutexAuth;
    int socket_fd;         
    int main_fd;            
    int id;                 // este Id no se refiere al valor asingando localmente en serverAuth.c en la creación del hilo, se refiere al valor obtenido de serverAuth() en main.c
} authMonitor;

ssize_t initMainMonitor(mainMonitor* m, int socket_fd);
ssize_t destroyMainMonitor(mainMonitor* m);

ssize_t initAuthMonitor(authMonitor* m, int client_sock, int main_sock, int id);
ssize_t destroyAuthMonitor(authMonitor* m);

#endif
#ifndef MONITORS_H
#define MONITORS_H

#include "main.h"


#define MAX_SERVERS_AUTH 3 //Maximo de servidores de autenticacion que se pueden conectar
#define MAX_FIREWALLS 3 //Maximo de firewalls que se pueden conectar 
#define CONDITION_EXIT 100 //Solicitudes que tiene que acumular los dos procesos concurrentes para finalizar

#define MAX_CLIENTS_AUTH 5 //Máximo de clientes para el servidor autenticación
#define MAX_CLIENTS_FIRE 5 //Máximo de clientes para el servidor firewall

typedef struct
{

    pthread_t thdsAuth[MAX_SERVERS_AUTH];
    pthread_t thdsFire[MAX_FIREWALLS];
    pthread_mutex_t mutexAuth, mutexFire;

    int socket_fd;
    int countAuth, countFire;
}mainMonitor;

typedef struct
{
    pthread_t thdsAuth[MAX_CLIENTS_AUTH];
    pthread_mutex_t mutexAuth;

    int socket_fd;
}authMonitor;

ssize_t initMainMonitor(mainMonitor* m, int socket_fd);
ssize_t destroyMainMonitor(mainMonitor* m);

ssize_t initAuthMonitor(authMonitor* m, int socket_fd);
ssize_t destroyAuthMonitor(authMonitor* m);


#endif
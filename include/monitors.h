#ifndef MONITORS_H
#define MONITORS_H

#include "main.h"

typedef struct
{
    pthread_t thdsAuth[MAX_SERVERS_AUTH];
    pthread_t thdsFire[MAX_FIREWALLS];
    pthread_mutex_t mutexAuth, mutexFire;

    int socket_fd;
    int countAuth, countFire;
}mainMonitor;

ssize_t initMainMonitor(mainMonitor* m, int socket_fd);
ssize_t destroyMainMonitor(mainMonitor* m);


#endif
#include "monitors.h"


ssize_t initMainMonitor(mainMonitor* m, int socket_fd)
{
    if (m == NULL)
        return -1;

    m->socket_fd = socket_fd;
    m->countAuth = 0;
    m->countFire = 0;

    if (pthread_mutex_init(&(m->mutexAuth), NULL) != 0)
        return -1;

    if (pthread_mutex_init(&(m->mutexFire), NULL) != 0)
        return -1;

    return 0;
}
#include "monitors.h"
#include "authServer.h"

//Acabo de darme cuenta que mi uso "ssize_t" porque yo no regreso la cantidad de bytes y mucho menos implemento herramientas que haga uso de eso
//me mal acostumbre a usarlo porque las funciones de sys/socket están repletas de valores de retorno de ssize_t
ssize_t initMainMonitor(mainMonitor* m, int socket_fd) {
    if (!m || m == NULL) 
        return -1;
    m->socket_fd = socket_fd;
    m->countAuth = 0;
    m->countFire = 0;
    if (pthread_mutex_init(&m->mutexAuth, NULL) != 0) 
        return -1;
    if (pthread_mutex_init(&m->mutexFire, NULL) != 0) 
        return -1;
    memset(usersHandle, 0, sizeof(usersHandle));
    return 0;
}

ssize_t destroyMainMonitor(mainMonitor* m) {
    if (!m || m == NULL)  
        return -1;
    pthread_mutex_destroy(&m->mutexAuth);
    pthread_mutex_destroy(&m->mutexFire);
    return 0;
}

ssize_t initAuthMonitor(authMonitor* m, int client_sock, int main_sock, int id) {
    if (!m || m == NULL) 
        return -1;
    m->socket_fd = client_sock;
    m->main_fd = main_sock;
    m->id = id;
    if (pthread_mutex_init(&m->mutexAuth, NULL) != 0)
        return -1;
    return 0;
}

ssize_t destroyAuthMonitor(authMonitor* m) {
    if (!m || m == NULL)
        return -1;
    pthread_mutex_destroy(&m->mutexAuth);
    return 0;
}
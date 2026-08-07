#ifndef MAIN_H
#define MAIN_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>


#define MAX_SERVERS_AUTH 3 //Maximo de servidores de autenticacion que se pueden conectar
#define MAX_FIREWALLS 3 //Maximo de firewalls que se pueden conectar 
#define CONDITION_EXIT 100 //Solicitudes que tiene que acumular los dos procesos concurrentes para finalizar

extern int countAuth, countFire;

pthread_t thdsAuth[MAX_SERVERS_AUTH];
pthread_t thdsFire[MAX_FIREWALLS];
pthread_mutex_t mutexAuth, mutexFire;
pthread_barrier_t barrierAuth, barrierFire;


void* authServer(void* arg);
void* firewall(void* arg);

#endif 


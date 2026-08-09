#ifndef AUTH_CLIENT_H
#define AUTH_CLIENT_H

#include "dependencies.h"
#include "ports.h"
#include "monitors.h"

void* connectSeveralToAuth(void* arg); //Por horribles desiciones de arquitectura y el hecho de que no lo pensé mucho (y que ya había avanzando y
                                       //no podía reconstruir todo) cada hilo en el cliente se conecta y cierra en un while sockets que van hacía
                                       //authServer, para hacer un send() a consult_user() (el cual también de mala manera crea y destruye continuamente
                                       //hilos para aceptar la conexiones)

#endif
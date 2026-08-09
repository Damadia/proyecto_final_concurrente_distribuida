#ifndef AUTH_SERVER_H
#define AUTH_SERVER_H

#include "dependencies.h"
#include "monitors.h"

#define VALID_USERS_AMOUNT 3 //Al final no tuvo uso, utilizo MAX_SERVER_AUTH

extern int usersHandle[VALID_USERS_AMOUNT][2]; // Este arreglo que luce como [[,],[,]...[,]], guarda, de acuerdo con el id asignado en la función
                                               // de authServer en main.c, el total de exitos (1ra posición) y fracasos (2da posición) de cada hilo
                                               // de la función handleClient() en authServer.c cada que un cliente en clients.c hace un recv()
                                               // que simula la autorización al servidor
extern const char* validUsers[VALID_USERS_AMOUNT][2]; //Extern, forma de declarar la declaración de variable entre archivos para que otro pueda hacer uso de ella

void* handleClientAuth(void* arg);
int consult_user(const char* user, const char* pass);

#endif
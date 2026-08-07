#ifndef PORTS_H
#define PORTS_H

extern int PORT_MAIN = 10001; //El puerto del main

#define PORT_AUTH 20001 //El puerto de los servidores de autenticacion (todos los hilos comparten el mismo fd socket)
#define PORT_FIRE 20002 //El puerto de los firewalls


#endif
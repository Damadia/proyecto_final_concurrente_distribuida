#ifndef MAIN_H
#define MAIN_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif






void* authServer(void* arg);
void* firewall(void* arg);

#endif 


#ifndef AUTH_SERVER_H
#define AUTH_SERVER_H

#include "dependencies.h"
#include "monitors.h"

extern authMonitor mon;

#define VALID_USERS_AMOUNT 3

const char* validUsers[][2] = {
    {"user123", "pass"},
    {"admin", "admin321"},
    {"client", "12345"}
};
int a[4];


void* handleClient(void* arg);
int consult_user(char* user, char* pass);



#endif
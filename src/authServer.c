#include "monitors.h"
#include "dependencies.h"
#include "ports.h"

int main(){
    int fdToMain;
    struct sockaddr_in addr_auth;
    socklen_t addr_len = sizeof(addr_auth);

    //crear socket
    fdToMain = socket(AF_INET, SOCK_STREAM, 0);

    if (fdToMain < 0)
    {
        perror("SA: Error al crear el socket\n");
        exit(EXIT_FAILURE);
    }

    addr_auth.sin_port = htons(PORT_MAIN);
    addr_auth.sin_family = AF_INET;

    //Crear la ip:puerto
    if (inet_pton(AF_INET, "127.0.0.1", &addr_auth.sin_addr) <= 0)
    {
        perror("SA: Ip invalida\n");
        exit(EXIT_FAILURE);
    }

    //conectarse al servidor
    int resCon;
    if ((resCon = connect(fdToMain, (struct sockaddr*)&addr_auth, addr_len)) != 0)
    {
        perror("SA: Error al conectarse al servidor");
        close(fdToMain);
        exit(EXIT_FAILURE);
    }



    return 0;
}
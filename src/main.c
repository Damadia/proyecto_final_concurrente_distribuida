#include "main.h"
#include "ports.h"
#include "monitors.h"


void* authServer(void* arg)
{
    int socket_fd = *((int*)arg);

    pthread_exit(NULL);

}

int main()
{   
    int opt, socketMain;
    struct sockaddr_in mainServer;
    socklen_t mainLen = sizeof(mainServer);
    char buffer[512];
    char* testMsh = "Mensaje del servidor principal";

    mainMonitor m;


    // Crear socket, las dos piezas del servidor van a compartir el mismo socket
    socketMain = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketMain < 0)
    {
        perror("Error al crear el socket publico");
        exit(EXIT_FAILURE);
    }

    // Crear la dirección principal

    mainServer.sin_family = AF_INET;
    mainServer.sin_port = htons(PORT_MAIN);
    mainServer.sin_addr.s_addr = INADDR_ANY;


    //Hacer el bind

    ssize_t bindRes = bind(socketMain,(struct sockaddr*)&mainServer, (socklen_t)mainLen);
    if (bindRes < 0)
    {
        perror("Error al hacer el enlace del socket");
        exit(EXIT_FAILURE);
    }


    //crear el monitor
    if (initMainMonitor(&m, socketMain) < 0)
    {
        perror("Error al crear el monitor del servidor principal");
        exit(EXIT_FAILURE);
    }
       

    return 0;
}
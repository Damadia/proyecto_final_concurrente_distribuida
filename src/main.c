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
    int opt, socketMain, acpRes;
    struct sockaddr_in mainServer;
    socklen_t mainLen = sizeof(mainServer);
    char buffer[512];
    char* testMsh = "Mensaje del servidor principal";

    mainMonitor m;


    // Crear socket, las dos piezas del servidor van a compartir el mismo socket
    socketMain = socket(AF_INET, SOCK_STREAM, 0);
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

    
    char myIp[INET_ADDRSTRLEN]; //Solo estoy haciendo una prueba
    int myPort;

    int num = getpeername(socketMain, (struct sockaddr*)&mainServer, &mainLen);
    if (num < 0)
    {
        perror("Error al obtener la dirección del peer");
        exit(EXIT_FAILURE);
    }

    myPort = htons(mainServer.sin_port);

    char* a;
    a = inet_ntop(AF_INET,&mainServer.sin_addr, myIp, (socklen_t)sizeof(myIp));
    if (a == NULL)
    {
        perror("Error al convertir la dirección IP a cadena");
        exit(EXIT_FAILURE);
    }

    printf("Dirección IP:Puerto = %s:%d\n",myIp, myPort);

    acpRes = accept(socketMain, (struct sockaddr*)&mainServer, &mainLen);
    if (acpRes < 0)
    {
        perror("Error al aceptar la primera conexión");
        exit(EXIT_FAILURE);
    }
    printf("Esto no se va a ejecutar");

    return 0;
}
#include "main.h"
#include "ports.h"
#include "monitors.h"
#include "dependencies.h"


void* authServer(void* arg)
{
    int* SF = malloc(sizeof(int)*2);//exitos y fracasos
    SF[0] = 10; //<- NO OLVIDES BORRAR ESTO
    SF[1] = 5;


    mainMonitor* m = (mainMonitor*)arg;
    struct sockaddr_in addr_server;
    socklen_t size_addr = sizeof(addr_server);

    char* msgSend = "SM to SA: Servidor de autenticación estableció comunicación con el servidor prinicipal\n";
    char rcvMsg[1024];
    memset(rcvMsg, "", strlen(rcvMsg));
    rcvMsg[1023] = "\0";

    char* ip;
    int port;

    //aceptar al servidor autenticación
    int fd = accept(m->socket_fd, (struct sockaddr*)&addr_server, &size_addr);
    if (fd < 0)
    {
        perror("SM: El servidor de autenticación tuvo un problema al conectarse, cerrando hilo");
        pthread_exit(NULL);
        return NULL;
    } 

    //Mandale un mensaje a authServer.c que ya está la conexión con un hilo
    if (send(fd, msgSend, (size_t)strlen(msgSend), 0) < 0)
    {
        perror("SM: El servidor de autenticación tuvo un error al mandar el mensaje, cerrando hilo");
        pthread_exit(NULL);
        close(fd);

        return NULL;
    }
    
    if(getpeername(fd, (struct sockaddr *)&addr_server, &size_addr) < 0)
    {
        perror("SM: Error al obtener la ip y puerto del servidor de autenticación");
        pthread_exit(NULL);
        close(fd);
        return NULL;
    }

    port = htons(addr_server.sin_port);
    ip = inet_ntop(AF_INET, &addr_server.sin_addr, ip, size_addr);
    if (ip == NULL)
    {
        perror("Error al traducir la IP del server auth");
    }

    printf("SM: ip-puerto del servidor de autenticación: %s:%d", ip, port);
    
    int* success;
    ssize_t successRcv = sizeof(success); //esto son los bytes recibidos donde 0 significa que el servidor fue apagado
    char abort[512];

    while (1)
    {
        printf("SM: Esperando respuesta del servidor de autenticación");
        successRcv = recv(fd, success, successRcv, 0);
        if (successRcv == 0)
        {
            printf("SM from SA: Cerre el servidor de autenticación");
            close(fd);

            pthread_exit(NULL);
            return NULL;
        }
        else if (successRcv < 0)
        {
            printf("SM: Un error ocurrió al recibir un mensaje");
        }

        printf("SM: Escriba 'bye' para cerrar el hilo que mantiene la comunicación con el servidor de autenticación\n");
        if (fgets(abort, strlen(abort), stdin) != NULL)
        {
            perror("Error en la captura de 'bye'\n");
            continue;
        }
        if (strcspn(abort, "bye") == 0)
        {
            printf("SM: Cerrando el servidor de autenticación");
            break;
        }
    }

    close(fd);
    pthread_exit((void*)SF);
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

    //escuchar
    int res;
    if ((res = listen(socketMain, 2)) < 0)
    {
        perror("Error al escuchar los servers");
        exit(EXIT_FAILURE);
    }

    //crear hilo para el servidor de autenticación
    void* server1_res;

    if (pthread_create(m.thdsAuth[0], NULL, authServer, (void*)&m) != 0)
    {
        perror("MS: Error al crear un hilo de autenticación\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_join(m.thdsAuth[0], &server1_res) != 0)
    {
        perror("MS: Error al devolver los resultados del servidor de autenticación");
        exit(EXIT_FAILURE);
    }


    if (server1_res == NULL)
    {
        perror("MS: Error en devolver los exitos y fracasos\n");
        exit(EXIT_FAILURE);
    }

    int* server1_res_values = (int*)server1_res;

    printf("MS: Relación exitos/fracasos: %f\n", server1_res_values[0] / server1_res_values[1]);


    return 0;
}
#include "authServer.h"
#include "monitors.h"
#include "ports.h"

// Global user database (defined here)
int usersHandle[VALID_USERS_AMOUNT][2];
const char* validUsers[VALID_USERS_AMOUNT][2] = {
    {"user123", "pass"},
    {"admin", "admin321"},
    {"client", "12345"}
};

// Function: validate credentials
int consult_user(const char* user, const char* pass) {
    for (int i = 0; i < VALID_USERS_AMOUNT; i++) {
        if (strcmp(user, validUsers[i][0]) == 0 &&
            strcmp(pass, validUsers[i][1]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Thread to handle one client connection (temporary)
void* handleClientAuth(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);

    char buffer[256];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer)-1, 0);
    if (n <= 0) {
        close(client_fd);
        int* res = malloc(sizeof(int));
        *res = 0;  // failure on error
        return res;
    }
    buffer[n] = '\0';
    // Remove newline
    buffer[strcspn(buffer, "\n")] = '\0';

    char user[64], pass[64];
    if (sscanf(buffer, "%s %s", user, pass) != 2) {
        const char* err = "Invalid format. Use: user pass\n";
        send(client_fd, err, strlen(err), 0);
        close(client_fd);
        int* res = malloc(sizeof(int));
        *res = 0;
        return res;
    }

    int success = consult_user(user, pass);
    const char* msg = success ? "Authentication SUCCESS\n" : "Authentication FAILED\n";
    send(client_fd, msg, strlen(msg), 0);
    close(client_fd);

    int* res = malloc(sizeof(int));
    *res = success ? 1 : 0;
    return res;
}

// The main thread that accepts clients and spawns temporary threads
void* handleClients(void* arg) {
    authMonitor* mon = (authMonitor*)arg;
    int listen_fd = mon->socket_fd;
    int main_fd = mon->main_fd;
    int id = mon->id;

    int success_count = 0, failure_count = 0;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &len);
        if (client_fd < 0) {
            // probably socket closed, break
            perror("authServer: accept");
            break;
        }

        // Spawn thread to handle this client
        int* fd_ptr = malloc(sizeof(int));
        *fd_ptr = client_fd;
        pthread_t temp_thread;
        pthread_create(&temp_thread, NULL, handleClientAuth, fd_ptr);
        int* result;
        pthread_join(temp_thread, (void**)&result);
        if (result) {
            int res = *result;
            free(result);
            // Send result to main server
            if (send(main_fd, &res, sizeof(res), 0) != sizeof(res)) {
                perror("authServer: send to main");
                close(client_fd);
                break;
            }
            if (res == 1) success_count++;
            else failure_count++;
        } else {
            // error, maybe close
            break;
        }
    }

    // Return counts
    int* ret = malloc(3 * sizeof(int));
    ret[0] = success_count;
    ret[1] = failure_count;
    ret[2] = id;
    return ret;
}

int main() {
    // 1. Connect to main server
    int fd_to_main = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_to_main < 0) {
        perror("authServer: socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in addr_main;
    memset(&addr_main, 0, sizeof(addr_main));
    addr_main.sin_family = AF_INET;
    addr_main.sin_port = htons(PORT_MAIN);
    if (inet_pton(AF_INET, "127.0.0.1", &addr_main.sin_addr) <= 0) {
        perror("authServer: inet_pton");
        close(fd_to_main);
        exit(EXIT_FAILURE);
    }
    if (connect(fd_to_main, (struct sockaddr*)&addr_main, sizeof(addr_main)) < 0) {
        perror("authServer: connect");
        close(fd_to_main);
        exit(EXIT_FAILURE);
    }

    // 2. Receive our assigned ID from main
    int id;
    ssize_t n = recv(fd_to_main, &id, sizeof(id), 0);
    if (n != sizeof(id)) {
        perror("authServer: recv id");
        close(fd_to_main);
        exit(EXIT_FAILURE);
    }
    printf("AuthServer: assigned ID %d\n", id);

    // 3. Create listening socket for clients (port = PORT_AUTH + id)
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("authServer: client socket");
        close(fd_to_main);
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr_client;
    memset(&addr_client, 0, sizeof(addr_client));
    addr_client.sin_family = AF_INET;
    addr_client.sin_addr.s_addr = INADDR_ANY;
    addr_client.sin_port = htons(PORT_AUTH + id);
    if (bind(listen_fd, (struct sockaddr*)&addr_client, sizeof(addr_client)) < 0) {
        perror("authServer: bind client");
        close(listen_fd);
        close(fd_to_main);
        exit(EXIT_FAILURE);
    }
    if (listen(listen_fd, MAX_CLIENTS_AUTH) < 0) {
        perror("authServer: listen client");
        close(listen_fd);
        close(fd_to_main);
        exit(EXIT_FAILURE);
    }
    printf("AuthServer %d listening on port %d\n", id, PORT_AUTH + id);

    // 4. Create the main handler thread
    authMonitor mon;
    if (initAuthMonitor(&mon, listen_fd, fd_to_main, id) < 0) {
        perror("authServer: initAuthMonitor");
        close(listen_fd);
        close(fd_to_main);
        exit(EXIT_FAILURE);
    }
    pthread_t handler_thread;
    pthread_create(&handler_thread, NULL, handleClients, &mon);

    // 5. Wait for handler to finish and get results
    int* results;
    pthread_join(handler_thread, (void**)&results);
    if (results) {
        int success = results[0];
        int failure = results[1];
        int server_id = results[2];
        printf("\n=== Auth Server %d Results ===\n", server_id);
        printf("Success: %d, Failure: %d, Total: %d\n", success, failure, success+failure);
        double ratio = (failure == 0) ? success : (double)success / failure;
        printf("Success/Failure ratio: %.2f\n", ratio);
        free(results);
    }

    // Cleanup
    destroyAuthMonitor(&mon);
    close(listen_fd);
    close(fd_to_main);
    return 0;
}
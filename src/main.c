#include "main.h"
#include "monitors.h"
#include "ports.h"

// Global variables for main process
int currentReq = 0;
pthread_mutex_t req_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barrier;
int server_results[MAX_SERVERS_AUTH][2];  // [success][failure] per server

void* authServerThread(void* arg) {
    int idx = *(int*)arg;
    free(arg);

    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(g_main_socket, (struct sockaddr*)&client_addr, &len);
    if (client_fd < 0) {
        perror("authServerThread: accept");
        return NULL;
    }

    // Send the assigned ID to the auth server
    if (send(client_fd, &idx, sizeof(idx), 0) != sizeof(idx)) {
        perror("authServerThread: send id");
        close(client_fd);
        return NULL;
    }

    printf("[Main] Auth server %d connected\n", idx);

    // Wait for all auth servers to connect
    pthread_barrier_wait(&barrier);

    int local_success = 0, local_failure = 0;
    int result;

    while (1) {
        ssize_t n = recv(client_fd, &result, sizeof(result), 0);
        if (n <= 0) {
            // connection closed or error
            break;
        }

        // Update global counters
        pthread_mutex_lock(&req_mutex);
        currentReq++;
        if (result == 1) {
            local_success++;
            server_results[idx][0]++;  // success
        } else {
            local_failure++;
            server_results[idx][1]++;  // failure
        }
        int total = currentReq;
        pthread_mutex_unlock(&req_mutex);

        if (total >= CONDITION_EXIT) {
            // signal all to stop (we'll close the socket to wake others)
            break;
        }
    }

    close(client_fd);
    // Return local counts to main
    int* ret = malloc(2 * sizeof(int));
    ret[0] = local_success;
    ret[1] = local_failure;
    return ret;
}

int main() {
    signal(SIGPIPE, SIG_IGN);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("main: socket");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_MAIN);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("main: bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, MAX_SERVERS_AUTH + MAX_FIREWALLS) < 0) {
        perror("main: listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    mainMonitor mon;
    if (initMainMonitor(&mon, server_fd) < 0) {
        perror("main: initMainMonitor");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    // store global socket for accept in threads
    g_main_socket = server_fd;

    // Initialize barrier
    pthread_barrier_init(&barrier, NULL, MAX_SERVERS_AUTH);

    // Create threads for each auth server
    pthread_t threads[MAX_SERVERS_AUTH];
    for (int i = 0; i < MAX_SERVERS_AUTH; i++) {
        int* idx = malloc(sizeof(int));
        *idx = i;
        if (pthread_create(&threads[i], NULL, authServerThread, idx) != 0) {
            perror("main: pthread_create");
            free(idx);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish and collect results
    int total_success = 0, total_failure = 0;
    int per_server_requests[MAX_SERVERS_AUTH] = {0};
    for (int i = 0; i < MAX_SERVERS_AUTH; i++) {
        int* res;
        pthread_join(threads[i], (void**)&res);
        if (res) {
            per_server_requests[i] = res[0] + res[1];
            total_success += res[0];
            total_failure += res[1];
            free(res);
        }
    }

    // Print results table
    printf("\n=== Main Server Results ===\n");
    printf("Total requests processed: %d (Success: %d, Failure: %d)\n",
           total_success + total_failure, total_success, total_failure);
    printf("\nPer Auth Server:\n");
    printf("Server\tRequests\tSuccess\tFailure\tSuccess/Total\n");
    for (int i = 0; i < MAX_SERVERS_AUTH; i++) {
        int req = per_server_requests[i];
        double ratio = (req > 0) ? (double)server_results[i][0] / req : 0.0;
        printf("%d\t%d\t\t%d\t%d\t%.2f\n",
               i, req, server_results[i][0], server_results[i][1], ratio);
    }

    // Cleanup
    pthread_barrier_destroy(&barrier);
    destroyMainMonitor(&mon);
    close(server_fd);
    return 0;
}

// Global variable for socket (used in threads)
int g_main_socket;
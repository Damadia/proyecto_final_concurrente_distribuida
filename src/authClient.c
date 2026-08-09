#include "authClient.h"
#include "monitors.h"
#include "ports.h"

void* connectSeveralToAuth(void* arg) {
    int thread_id = *(int*)arg;
    free(arg);

    srand(time(NULL) + thread_id);  // seed per thread

    while (1) {
        // Choose a random auth server ID (0..MAX_SERVERS_AUTH-1)
        int auth_id = rand() % MAX_SERVERS_AUTH;
        int port = PORT_AUTH + auth_id;

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("client: socket");
            break;
        }
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
            perror("client: inet_pton");
            close(sock);
            break;
        }
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            // server probably closed, exit
            perror("client: connect");
            close(sock);
            break;
        }

        // Generate random result: 1/3 failure (0), 2/3 success (1)
        int result = (rand() % 3 == 0) ? 0 : 1;
        // Send the result (as int) to auth server
        if (send(sock, &result, sizeof(result), 0) != sizeof(result)) {
            perror("client: send");
            close(sock);
            break;
        }

        // Optionally read response (we ignore)
        close(sock);

        // Sleep 3-4 seconds
        int sleep_time = 3 + (rand() % 2);
        sleep(sleep_time);
    }
    return NULL;
}

int main() {
    printf("Clients starting...\n");
    pthread_t threads[MAX_SERVERS_AUTH];
    for (int i = 0; i < MAX_SERVERS_AUTH; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&threads[i], NULL, connectSeveralToAuth, id);
    }
    for (int i = 0; i < MAX_SERVERS_AUTH; i++) {
        pthread_join(threads[i], NULL);
    }
    printf("All clients finished.\n");
    return 0;
}
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 700
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];

    char welcome[] = "Welcome to the a totally legitamate server! Enter some text:\n";
    send(client_socket, welcome, sizeof(welcome), 0);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0) {
            break;
        }

        // Vulnerability 1: Buffer overflow
        char response[BUFFER_SIZE];
        strcpy(response, buffer);

        // Vulnerability 2: Format string vulnerability
        printf(buffer);

        // Vulnerability 3: Integer overflow
        int x = atoi(buffer);
        x = x * 100000;
        printf("Integer Overflow Test: x * 100000 = %d\n", x);
        
        // Vulnerability 4: Null pointer dereference
        
        if (strcmp(buffer, "null") == 0) {
            char *null_pointer = NULL;
            printf("Null pointer dereference test: %c\n", *null_pointer); 
        }

         // Vulnerability 5: Double Free
        if (strcmp(buffer, "free") == 0) {
            char *double_free_ptr = malloc(16);
            strcpy(double_free_ptr, "Double Free Test");
            free(double_free_ptr);
            free(double_free_ptr); 
        }

        send(client_socket, response, bytes_received, 0);
    }

    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(1);
    }

    int enable_reuse = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(enable_reuse)) < 0) {
        perror("setsockopt(SO_REUSEADDR)");
        exit(1);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_socket, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port 700 \n");

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);

        if (client_socket == -1) {
            perror("accept");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
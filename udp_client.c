#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Create UDP socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Send test message
    char *message = "TEST MESSAGE";
    sendto(client_socket, message, strlen(message), 0, 
           (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Receive response
    socklen_t addr_len = sizeof(server_addr);
    int recv_len = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, 
                            (struct sockaddr*)&server_addr, &addr_len);
    if (recv_len < 0) {
        perror("Receive failed");
    } else {
        buffer[recv_len] = '\0'; // Null-terminate received string
        printf("Server Response: %s\n", buffer);
    }

    close(client_socket);
    return 0;
}


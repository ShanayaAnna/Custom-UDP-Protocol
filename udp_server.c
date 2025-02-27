#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Create UDP socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET; //sets the address family to IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; //allows the server to listen on all available network interfaces (e.g., localhost, Ethernet, Wi-Fi).
    server_addr.sin_port = htons(SERVER_PORT); //converts SERVER_PORT (a plain integer) into network byte order (big-endian

    // Bind socket to the server address
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Receiving data from client
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int recv_len = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, 
                                (struct sockaddr*)&client_addr, &addr_len);
        if (recv_len < 0) {
            perror("Receive failed");
            continue;
        }

        printf("Received from client: %s\n", buffer);

        // Send response
        char *response = "ACK TEST RESPONSE";
        sendto(server_socket, response, strlen(response), 0, 
               (struct sockaddr*)&client_addr, addr_len);
    }

    close(server_socket);
    return 0;
}

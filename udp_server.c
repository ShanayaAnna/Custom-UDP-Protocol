#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.h"

#define SERVER_PORT 12345
#define BUFFER_SIZE sizeof(DataPacket)

void process_packet(DataPacket *packet) {
    if (packet->start_id != START_OF_PACKET_ID || packet->end_id != END_OF_PACKET_ID) {
        printf("Received corrupt packet! Ignoring...\n");
        return;
    }
    printf("Received Packet %d from Client %d: \"%s\"\n", packet->segment_no, packet->client_id, packet->payload);
    //print_packet(packet);
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    DataPacket packet;

    // Create UDP socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Receiving data from client
    while (1) {
        ssize_t received = recvfrom(server_socket, &packet, BUFFER_SIZE, 0, 
                                    (struct sockaddr*)&client_addr, &client_addr_len);
        if (received < 0) {
            perror("Receive failed");
            continue;
        }

        // Process received packet
        process_packet(&packet);
    }

    close(server_socket);
    return 0;
}

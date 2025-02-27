#include "packet.h"
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define MAX_RETRIES 3
#define TIMEOUT 3  // in seconds

// Function to wait for ACK from the server
int wait_for_ack(int sock, struct sockaddr_in *server_addr, socklen_t *server_addr_len) {
    DataPacket ack_packet;
    struct timeval tv;
    tv.tv_sec = TIMEOUT;  // Set timeout duration
    tv.tv_usec = 0;

    // Set socket timeout
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    ssize_t received = recvfrom(sock, &ack_packet, sizeof(DataPacket), 0,
                                 (struct sockaddr*)server_addr, server_addr_len);
    if (received < 0) {
        return 0;  // Timeout or error
    }

    // Check if it's an ACK
    if (ack_packet.packet_type == PACKET_TYPE_ACK) {
        printf("ACK received for packet %d\n", ack_packet.segment_no);
        return 1;  // ACK received
    }

    return 0;  // Not an ACK, handle other cases (REJECT, etc.)
}

// Function to send a packet and wait for acknowledgment
int send_packet_and_wait_for_ack(int sock, struct sockaddr_in *server_addr, socklen_t *server_addr_len, unsigned char client_id, unsigned char segment_no, const char *message) {
    int retries = 0;
    DataPacket packet;
    create_data_packet(&packet, client_id, segment_no, message);

    // Serialize the packet into a buffer before sending
    unsigned char buffer[sizeof(DataPacket)];
    serialize_data_packet(&packet, buffer);

    while (retries < MAX_RETRIES) {
        // Send the packet
        ssize_t sent = sendto(sock, buffer, sizeof(DataPacket), 0, 
                              (struct sockaddr*)server_addr, sizeof(*server_addr));
        if (sent < 0) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }

        printf("Sent Packet %d: \"%s\"\n", segment_no, message);
        print_packet(&packet);

        // Wait for ACK
        if (wait_for_ack(sock, server_addr, server_addr_len)) {
            return 1;  // ACK received, exit loop
        }

        printf("ACK not received, retrying...\n");
        retries++;
    }

    printf("Server does not respond\n");
    return 0;  // Max retries reached without receiving ACK
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(server_addr);

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

    // Send 5 properly formatted packets and wait for acknowledgment
    for (int i = 0; i < 5; i++) {
        char message[30];
        snprintf(message, sizeof(message), "Message %d", i);
        if (!send_packet_and_wait_for_ack(client_socket, &server_addr, &server_addr_len, 1, i, message)) {
            close(client_socket);
            return 0;
        }
        sleep(1);  // Wait a bit before sending the next packet
    }

    close(client_socket);
    return 0;
}

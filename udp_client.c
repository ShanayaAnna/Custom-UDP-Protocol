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

// Function to send a packet and wait for acknowledgment
int send_packet_and_wait_for_ack(int client_socket, struct sockaddr_in *server_addr, socklen_t *server_addr_len, unsigned char client_id, unsigned char segment_no, const char *message) {
    int retries = 0;
    DataPacket data_packet;
    create_data_packet(&data_packet, client_id, segment_no, message);


    while (retries < MAX_RETRIES) {
        // Serialize DATA packet
        unsigned char data_buffer[sizeof(DataPacket)];
        memcpy(data_buffer, &data_packet, sizeof(DataPacket));
        printf("DATA Serialized Successfully\n");
    
        // Send the packet
        ssize_t sent = sendto(client_socket, data_buffer, sizeof(DataPacket), 0, 
                              (struct sockaddr*)server_addr, sizeof(*server_addr));
        if (sent < 0) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
    
        printf("Sent Packet %d: \"%s\"\n", segment_no, message);
        print_packet(&data_packet);
    
        // Receive ACK
        unsigned char ack_buffer[sizeof(AckPacket)];
        ssize_t received = recvfrom(client_socket, ack_buffer, sizeof(AckPacket), 0,
                                     (struct sockaddr*)server_addr, server_addr_len);
        if (received > 0) {
            AckPacket ack_packet;
            memcpy(&ack_packet, ack_buffer, sizeof(AckPacket));
            if (ack_packet.packet_type == PACKET_TYPE_ACK) {
                printf("ACK received for packet %d\n\n", ack_packet.received_segment_no);
                return 1;
            }
        }
    
        printf("ACK not received, retrying...\n");
        retries++;
    }
    
    printf("Server does not respond\n\n");
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

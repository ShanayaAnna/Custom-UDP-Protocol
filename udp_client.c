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
        print_data_packet(&data_packet);
    
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

// Test Case 1: Normal Packet Flow (No Errors)
void test_normal_packet_flow(int client_socket, struct sockaddr_in *server_addr, socklen_t *server_addr_len) {
    for (int i = 0; i < 5; i++) {
        char message[30];
        snprintf(message, sizeof(message), "Message %d", i);
        if (!send_packet_and_wait_for_ack(client_socket, server_addr, server_addr_len, 1, i, message)) {
            return;
        }
        sleep(1);
    }
}

// Test Case 2: Out-of-Sequence Packets
void test_out_of_sequence(int client_socket, struct sockaddr_in *server_addr, socklen_t *server_addr_len) {
    for (int i = 0; i < 3; i++) {
        char message[30];
        snprintf(message, sizeof(message), "Message %d", i);
        if (i == 0) {
            send_packet_and_wait_for_ack(client_socket, server_addr, server_addr_len, 1, 3, message);  // Send 3rd first
        } else if (i == 1) {
            send_packet_and_wait_for_ack(client_socket, server_addr, server_addr_len, 1, 1, message);  // Send 1st second
        } else {
            send_packet_and_wait_for_ack(client_socket, server_addr, server_addr_len, 1, 2, message);  // Send 2nd third
        }
        sleep(1);
    }
}

// Test Case 3: Duplicate Packets
void test_duplicate_packets(int client_socket, struct sockaddr_in *server_addr, socklen_t *server_addr_len) {
    for (int i = 0; i < 5; i++) {
        char message[30];
        snprintf(message, sizeof(message), "Message %d", i);
        
        send_packet_and_wait_for_ack(client_socket, server_addr, server_addr_len, 1, i, message);  // Send once
        send_packet_and_wait_for_ack(client_socket, server_addr, server_addr_len, 1, i, message);  // Send again (duplicate)
        sleep(1);
    }
}

// Test Case 4: Incorrect Packet Length
void test_incorrect_packet_length(int client_socket, struct sockaddr_in *server_addr, socklen_t *server_addr_len) {
    for (int i = 0; i < 5; i++) {
        char message[30];
        snprintf(message, sizeof(message), "Message %d", i);

        DataPacket data_packet;
        create_data_packet(&data_packet, 1, i, message);
        
        // Manually change the length to simulate an error
        data_packet.length = strlen(data_packet.payload) + 1;  // Make the length incorrect
        
        unsigned char data_buffer[sizeof(DataPacket)];
        memcpy(data_buffer, &data_packet, sizeof(DataPacket));
        
        sendto(client_socket, data_buffer, sizeof(DataPacket), 0, (struct sockaddr*)server_addr, sizeof(*server_addr));
        sleep(1);
    }
}

// Test Case 5: Invalid End ID
void test_invalid_end_id(int client_socket, struct sockaddr_in *server_addr, socklen_t *server_addr_len) {
    for (int i = 0; i < 5; i++) {
        char message[30];
        snprintf(message, sizeof(message), "Message %d", i);

        DataPacket data_packet;
        create_data_packet(&data_packet, 1, i, message);
        
        // Manually change the end_id to simulate an error
        data_packet.end_id = 0xFF;  // Invalid end ID
        
        unsigned char data_buffer[sizeof(DataPacket)];
        memcpy(data_buffer, &data_packet, sizeof(DataPacket));
        
        sendto(client_socket, data_buffer, sizeof(DataPacket), 0, (struct sockaddr*)server_addr, sizeof(*server_addr));
        sleep(1);
    }
}

// Main Function to Select Test Case
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

    // Ask user to select the test case
    int test_case;
    printf("Select test case to run (1-5):\n");
    printf("1. Normal Packet Flow (No Errors)\n");
    printf("2. Out-of-Sequence Packets\n");
    printf("3. Duplicate Packets\n");
    printf("4. Incorrect Packet Length\n");
    printf("5. Invalid End ID\n");
    printf("Enter your choice: ");
    scanf("%d", &test_case);

    // Run selected test case
    switch (test_case) {
        case 1:
            test_normal_packet_flow(client_socket, &server_addr, &server_addr_len);
            break;
        case 2:
            test_out_of_sequence(client_socket, &server_addr, &server_addr_len);
            break;
        case 3:
            test_duplicate_packets(client_socket, &server_addr, &server_addr_len);
            break;
        case 4:
            test_incorrect_packet_length(client_socket, &server_addr, &server_addr_len);
            break;
        case 5:
            test_invalid_end_id(client_socket, &server_addr, &server_addr_len);
            break;
        default:
            printf("Invalid test case selection.\n");
            break;
    }

    close(client_socket);
    return 0;
}

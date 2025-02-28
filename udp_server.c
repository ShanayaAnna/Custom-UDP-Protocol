#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.h"

#define SERVER_PORT 12345
#define BUFFER_SIZE sizeof(DataPacket)

void process_packet(DataPacket *data_packet, struct sockaddr_in *client_addr, socklen_t client_addr_len, int server_socket, int *expected_segment) {
    if (data_packet->start_id != START_OF_PACKET_ID) {
        printf("Received corrupt packet! Ignoring...\n");
        return;
    }

    // Error Handling
    RejectPacket reject_packet;
    int send_reject = 0;

    if (data_packet->segment_no < *expected_segment) {
        printf("Duplicate packet detected: %d\n", data_packet->segment_no);
        create_reject_packet(&reject_packet, data_packet->client_id, REJECT_DUPLICATE, data_packet->segment_no);
        send_reject = 1;
    } 
    else if (data_packet->segment_no > *expected_segment) {
        printf("Out-of-sequence packet detected: %d (expected %d)\n", data_packet->segment_no, *expected_segment);
        create_reject_packet(&reject_packet, data_packet->client_id, REJECT_OUT_OF_SEQUENCE, data_packet->segment_no);
        send_reject = 1;
    } 
    else if (data_packet->length != strlen(data_packet->payload)) {
        printf("Incorrect length in packet %d\n", data_packet->segment_no);
        create_reject_packet(&reject_packet, data_packet->client_id, REJECT_LENGTH_MISMATCH, data_packet->segment_no);
        send_reject = 1;
    } 
    else if (data_packet->end_id != END_OF_PACKET_ID) {
        printf("End ID mismatch in packet %d\n", data_packet->segment_no);
        create_reject_packet(&reject_packet, data_packet->client_id, REJECT_END_MISSING, data_packet->segment_no);
        send_reject = 1;
    }

    // If any error was detected, send a reject packet
    if (send_reject) {
        // Serialize REJECT packet
        unsigned char reject_buffer[sizeof(RejectPacket)];
        memcpy(reject_buffer, &reject_packet, sizeof(RejectPacket));

        // Send the REJECT packet
        ssize_t sent = sendto(server_socket, reject_buffer, sizeof(RejectPacket), 0,
                            (struct sockaddr*)client_addr, client_addr_len);
        if (sent < 0) {
            perror("Send REJECT failed");
            return;
        }
        printf("Sent REJECT for packet %d\n\n", data_packet->segment_no);
        return;
    }

    //Otherwise send an ACK Packet
    printf("Received Packet %d from Client %d: \"%s\"\n", data_packet->segment_no, data_packet->client_id, data_packet->payload);

    // Create ACK packet
    AckPacket ack_packet;
    create_ack_packet(&ack_packet, data_packet->client_id, data_packet->segment_no);

    // Serialize ACK packet
    unsigned char ack_buffer[sizeof(AckPacket)];
    memcpy(ack_buffer, &ack_packet, sizeof(AckPacket));

    // Send the ACK packet
    ssize_t sent = sendto(server_socket, ack_buffer, sizeof(AckPacket), 0,
                          (struct sockaddr*)client_addr, client_addr_len);
    if (sent < 0) {
        perror("Send ACK failed");
        return;
    }

    printf("Sent ACK for Packet %d\n\n", data_packet->segment_no);

    // Update expected segment number
    (*expected_segment)++;
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    DataPacket data_packet;
    int expected_segment = 0;

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
        ssize_t received = recvfrom(server_socket, &data_packet, sizeof(DataPacket), 0, 
                                    (struct sockaddr*)&client_addr, &client_addr_len);
        if (received < 0) {
            perror("Receive failed");
            continue;
        }

        // Process received packet and send ACK or Reject
        process_packet(&data_packet, &client_addr, client_addr_len, server_socket, &expected_segment);
    }

    close(server_socket);
    return 0;
}

#include <stdio.h>
#include <string.h>
#include "packet.h"

// Function to initialize a DataPacket
void create_data_packet(DataPacket *packet, uint8_t client_id, uint8_t segment_no, const char *payload) {
    packet->start_id = START_OF_PACKET_ID;
    packet->client_id = client_id;
    packet->packet_type = PACKET_TYPE_DATA;
    packet->segment_no = segment_no;
    packet->length = strlen(payload);
    strncpy(packet->payload, payload, sizeof(packet->payload));
    packet->end_id = END_OF_PACKET_ID;
}

// Function to initialize an ACK Packet
void create_ack_packet(AckPacket *ack_packet, uint8_t client_id, unsigned char segment_no) {
    ack_packet->start_id = START_OF_PACKET_ID;
    ack_packet->client_id = client_id;
    ack_packet->packet_type = PACKET_TYPE_ACK;
    ack_packet->received_segment_no = segment_no;
    ack_packet->end_id = END_OF_PACKET_ID;
}


// Function to print a packet (for debugging purposes)
void print_packet(const DataPacket *packet) {
    printf("Packet Details:\n");
    printf("Start ID: 0x%X\n", packet->start_id);
    printf("Client ID: %d\n", packet->client_id);
    printf("Packet Type: 0x%X\n", packet->packet_type);
    printf("Segment No: %d\n", packet->segment_no);
    printf("Length: %d\n", packet->length);
    printf("Payload: %s\n", packet->payload);
    printf("End ID: 0x%X\n", packet->end_id);
}

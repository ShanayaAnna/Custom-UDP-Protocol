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

void create_reject_packet(RejectPacket *reject_packet, uint8_t client_id, unsigned short reject_code, unsigned char segment_no){
    reject_packet->start_id = START_OF_PACKET_ID;
    reject_packet->client_id = client_id;
    reject_packet->packet_type = PACKET_TYPE_REJECT;
    reject_packet->reject_sub_code = reject_code;
    reject_packet->received_segment_no = segment_no;
    reject_packet->end_id = END_OF_PACKET_ID;
}


// Function to print a packet (for debugging purposes)
void print_data_packet(const DataPacket *data_packet) {
    printf("Packet Details:\n");
    printf("Start ID: 0x%X\n", data_packet->start_id);
    printf("Client ID: %d\n", data_packet->client_id);
    printf("Packet Type: 0x%X\n", data_packet->packet_type);
    printf("Segment No: %d\n", data_packet->segment_no);
    printf("Length: %d\n", data_packet->length);
    printf("Payload: %s\n", data_packet->payload);
    printf("End ID: 0x%X\n", data_packet->end_id);
    
}

// Function to print a Reject Packet (for debugging)
void print_reject_packet(const RejectPacket *reject_packet) {
    printf("Reject Packet Details:\n");
    printf("Start ID: 0x%X\n", reject_packet->start_id);
    printf("Client ID: %d\n", reject_packet->client_id);
    printf("Packet Type: 0x%X\n", reject_packet->packet_type);
    printf("Reject Code: 0x%X\n", reject_packet->reject_sub_code);
    printf("Segment No: %d\n", reject_packet->received_segment_no);
    printf("End ID: 0x%X\n", reject_packet->end_id);
}



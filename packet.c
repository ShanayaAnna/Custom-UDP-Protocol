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

// Function to serialize a DataPacket into a byte array for transmission
void serialize_data_packet(DataPacket *packet, uint8_t *buffer) {
    memcpy(buffer, packet, sizeof(DataPacket));
}

// Function to deserialize a byte array back into a DataPacket
void deserialize_data_packet(uint8_t *buffer, DataPacket *packet) {
    memcpy(packet, buffer, sizeof(DataPacket));
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

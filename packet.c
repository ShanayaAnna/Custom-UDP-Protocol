#include "packet.h"

void create_data_packet(DataPacket *packet, unsigned char client_id, unsigned char segment_no, const char *message) {
    memset(packet, 0, sizeof(DataPacket));

    packet->start_id = htons(START_OF_PACKET_ID);
    packet->client_id = client_id;
    packet->packet_type = htons(PACKET_TYPE_DATA);
    packet->segment_no = segment_no;
    packet->length = strlen(message);
    strncpy(packet->payload, message, MAX_PAYLOAD_SIZE);
    packet->end_id = htons(END_OF_PACKET_ID);
}

void print_packet(const DataPacket *packet) {
    printf("Packet Details:\n");
    printf("Start ID: 0x%X\n", ntohs(packet->start_id));
    printf("Client ID: %d\n", packet->client_id);
    printf("Packet Type: 0x%X\n", ntohs(packet->packet_type));
    printf("Segment No: %d\n", packet->segment_no);
    printf("Length: %d\n", packet->length);
    printf("Payload: %s\n", packet->payload);
    printf("End ID: 0x%X\n", ntohs(packet->end_id));
}

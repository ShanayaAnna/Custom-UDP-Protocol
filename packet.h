#ifndef PACKET_H
#define PACKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_PAYLOAD_SIZE 255

// Packet Identifiers
#define START_OF_PACKET_ID 0xFFFF
#define END_OF_PACKET_ID 0xFFFF

// Packet Types
#define PACKET_TYPE_DATA 0xFFF1
#define PACKET_TYPE_ACK  0xFFF2
#define PACKET_TYPE_REJECT 0xFFF3

// Reject Sub Codes
#define REJECT_OUT_OF_SEQUENCE 0xFFF4
#define REJECT_LENGTH_MISMATCH 0xFFF5
#define REJECT_END_MISSING 0xFFF6
#define REJECT_DUPLICATE 0xFFF7

// Data Packet Structure
typedef struct {
    unsigned short start_id;
    unsigned char client_id;
    unsigned short packet_type;
    unsigned char segment_no;
    unsigned char length;
    char payload[MAX_PAYLOAD_SIZE];
    unsigned short end_id;
} DataPacket;

// ACK Packet Structure
typedef struct {
    unsigned short start_id;
    unsigned char client_id;
    unsigned short packet_type;
    unsigned char received_segment_no;
    unsigned short end_id;
} AckPacket;

// Reject Packet Structure
typedef struct {
    unsigned short start_id;
    unsigned char client_id;
    unsigned short packet_type;
    unsigned short reject_sub_code;
    unsigned char received_segment_no;
    unsigned short end_id;
} RejectPacket;

// Function prototypes
void create_data_packet(DataPacket *packet, unsigned char client_id, unsigned char segment_no, const char *message);
void print_packet(const DataPacket *packet);

#endif // PACKET_H

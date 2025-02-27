#include "packet.h"
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345

void send_packet(int sock, struct sockaddr_in *server_addr, unsigned char client_id, unsigned char segment_no, const char *message) {
    DataPacket packet;
    create_data_packet(&packet, client_id, segment_no, message);

    // Serialize the packet into a buffer before sending
    unsigned char buffer[sizeof(DataPacket)];
    serialize_data_packet(&packet, buffer);

    sendto(sock, buffer, sizeof(DataPacket), 0, 
           (struct sockaddr*)server_addr, sizeof(*server_addr));

    printf("Sent Packet %d: \"%s\"\n", segment_no, message);
    print_packet(&packet);
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

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

    // Send 5 properly formatted packets
    for (int i = 0; i < 5; i++) {
        char message[30];
        snprintf(message, sizeof(message), "Message %d", i);
        send_packet(client_socket, &server_addr, 1, i, message);
        sleep(1);
    }

    close(client_socket);
    return 0;
}


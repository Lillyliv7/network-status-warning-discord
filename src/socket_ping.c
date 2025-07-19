#include <socket_ping.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

// Function to check if a specific IP and port is accepting connections
bool check_ip_port(const char *ip, int port, int timeout_sec) {
    int sock;
    struct sockaddr_in server_addr;
    struct timeval timeout;

    // Create a socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set up the timeout for the socket
    timeout.tv_sec = timeout_sec;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        perror("Setting timeout failed");
        close(sock);
        return false;
    }

    // Set up the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convert IP address from text to binary form
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sock);
        return false;
    }

    // Attempt to connect to the server with timeout
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        return false;  // Connection failed
    }

    // Connection successful
    close(sock);
    return true;  // Port is open
}

#include <ping.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>


// Function to calculate checksum
unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// Ping function
bool ping(const char *ip_addr) {
    int sockfd;
    struct sockaddr_in addr;
    struct icmp_hdr icmp_pkt;
    char packet[PACKET_SIZE];
    int addr_len = sizeof(addr);
    struct timeval timeout = {0, 500000};  // 0.5 second timeout

    // Create raw socket
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // Set socket options
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // Configure the destination address
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_addr);

    // Fill ICMP packet
    memset(&icmp_pkt, 0, sizeof(icmp_pkt));
    icmp_pkt.type = ICMP_ECHO;
    icmp_pkt.code = 0;
    icmp_pkt.id = getpid();
    icmp_pkt.seq = 1;
    icmp_pkt.checksum = checksum(&icmp_pkt, sizeof(icmp_pkt));

    // Send the ICMP packet
    if (sendto(sockfd, &icmp_pkt, sizeof(icmp_pkt), 0, (struct sockaddr*)&addr, sizeof(addr)) <= 0) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }

    // Receive the ICMP reply
    if (recvfrom(sockfd, packet, PACKET_SIZE, 0, (struct sockaddr*)&addr, (socklen_t*)&addr_len) <= 0) {
        //timeout
        return false;
    } else {
        //success
        return true;
    }

    close(sockfd);
}

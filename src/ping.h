#pragma once


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

#include "discord.h"

// Define packet size
#define PACKET_SIZE 64

// ICMP header structure
struct icmp_hdr {
    uint8_t type;        // message type
    uint8_t code;        // message code
    uint16_t checksum;   // checksum
    uint16_t id;         // identifier
    uint16_t seq;        // sequence number
};

// Function to calculate checksum
unsigned short checksum(void *b, int len);

// Ping function
bool ping(const char *ip_addr);
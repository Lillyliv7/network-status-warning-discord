#pragma once

struct list_ip{
    bool dead;
    char *ip;
    char *host;
};
typedef struct list_ip list_ip;

#define SETTINGS_WEBHOOK_POS 0
#define SETTINGS_ID_POS 1
#define SETTINGS_DIE_STRING_POS 2
#define SETTINGS_ALIVE_STRING_POS 3
#define SETTINGS_IP_COUNT_POS 4
#define SETTINGS_HOSTS_POS 5

// The 1st item is the discord webhook url
// The 2nd item is your discord id surrounded by <@ > to ping you\
// The 3rd item is the string to send when an ip dies, it needs to have 2 "%s"'s in it, the first being the ip name+address and the 2nd being your discord id
// The 4th item is the same as the 3rd but sent when an ip comes back one
// The 5th item is how many ip addresses you are tracking
// The 6th item and up is formatted as <ip name>@<ip address> for each server you want to monitor

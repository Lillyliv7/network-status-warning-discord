#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#include "discord.h"
#include "ping.h"
#include "strsplit.h"
#include "define.h"
#include "settings_file.h"
#include "socket_ping.h"

int check_args(int argc, char *argv[]) {
    if (argc == 1) {
        puts("Error no settings file specified\nUsage: disc-serv-stat-alert ./settings.txt");
        return 1;
    }
    if (argc > 2) {
        puts("Error too many arguments specified\nUsage: disc-serv-stat-alert ./settings.txt");
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {

    // int result = check_ip_port("192.168.4.77", 23);

    // if (result == 1) {
    //     printf("Connection successful: %s:%d is accepting connections.\n","192.168.4.77", 22);
    // } else {
    //     printf("Connection failed: %s:%d is not accepting connections.\n","192.168.4.77", 22);
    // }

    if(check_args(argc, argv) != 0) return 1;

    char* settings_file_buf = read_settings_file(argv[1]);
    if (settings_file_buf == NULL) return 1;

    // handle settings file
    // split settings array
    char **settings_array;
    settings_array = str_split(settings_file_buf, '~');
    
    free(settings_file_buf);

    // set discord values from settings
    char *webhook_url = settings_array[SETTINGS_WEBHOOK_POS];
    char *discord_id = settings_array[SETTINGS_ID_POS];

    unsigned int ip_count = atoi(settings_array[SETTINGS_IP_COUNT_POS]);
    printf("%d\n", ip_count);

    list_ip *ip_list = malloc(sizeof(list_ip) * ip_count);

    // create array of ips and data to loop through in main loop to prevent
    // proccessing data multiple times for no reason
    for (int i = 0; i < ip_count; i++) {
        ip_list[i].dead = false;
        char **ip_arr;
        ip_arr = str_split(settings_array[i+SETTINGS_HOSTS_POS], '@');
        printf("%s@%s\n", ip_arr[0], ip_arr[1]);
        ip_list[i].host = ip_arr[0];
        ip_list[i].ip = ip_arr[1];
        free(ip_arr);
    }

    puts("main loop starting");

    for(;;) {
        for (int i = 0; i < ip_count; i++) {
            char *message = malloc(500);
            bool ping_succeeded;
            // icmp ping if no port specified, socket connect if port specified
            if (strchr(ip_list[i].ip, ':') != NULL) {
                // port specified
                int ip_len = strlen(ip_list[i].ip);
                char *inp_ip = malloc(ip_len);
                memcpy(inp_ip,ip_list[i].ip,ip_len);

                char** port_ip_arr = str_split(inp_ip, ':');
                ping_succeeded = check_ip_port(port_ip_arr[0], atoi(port_ip_arr[1]), 1);
                free(inp_ip);
            } else {
                // port not specified
                ping_succeeded = ping(ip_list[i].ip);
            }

            // if the ping did not succeed and we havent already warned of an outage, send a message to alert server dead
            if (ping_succeeded == false && ip_list[i].dead == false) {
                sprintf(message, settings_array[SETTINGS_DIE_STRING_POS], ip_list[i].host, ip_list[i].ip, settings_array[SETTINGS_ID_POS]);
                puts(message);
                send_discord_message(webhook_url, message);
                ip_list[i].dead = true;
            }

            // if the ping succeeded but we have reported the server as down, send a message to alert server alive
            else if (ping_succeeded == true && ip_list[i].dead == true) {
                sprintf(message, settings_array[SETTINGS_ALIVE_STRING_POS], ip_list[i].host, ip_list[i].ip, settings_array[SETTINGS_ID_POS]);
                puts(message);
                send_discord_message(webhook_url, message);
                ip_list[i].dead = false;
            }
            sleep(4);
        }
    }

    free(ip_list);

    return 0;
}

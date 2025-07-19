#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

#include <discord.h>
#include <ping.h>
#include <strsplit.h>
#include <define.h>
#include <settings_file.h>
#include <socket_ping.h>

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

char* webhook_url = 0;
char* ping_id = 0;

struct server {
    char* name;
    char* ip;
    int port;
    bool portSpecified;
    bool prevOnline;
}; typedef struct server server_t;

server_t* servers;
size_t elems_in_servers_arr = 0;
size_t curr_sizeof_servers = 0;

const char* discordOnlineMsg = "<@%s> ALERT SERVER %s@%s:%d IS NOW ONLINE";
const char* discordOfflineMsg = "<@%s> ALERT SERVER %s@%s:%d IS NOW OFFLINE";

int main(int argc, char *argv[]) {
    // int result = check_ip_port("192.168.4.77", 5863, 5);
    // printf("%d\n", result);
    if(check_args(argc, argv) != 0) return 1;

    char* settings_file_buf = read_settings_file(argv[1]);
    if (settings_file_buf == NULL) return 1;

    // handle settings file
    // split settings array
    char **settings_array;
    settings_array = str_split(settings_file_buf, '\n');

    // allocate space for 10 servers
    servers = malloc(sizeof(server_t)*10);
    if(!servers) {
        printf("ERROR COULD NOT ALLOCATE %ld BYTES\n", sizeof(server_t)*10);
        return 1;
    }
    curr_sizeof_servers = 10;
    
    // loop through every line in the settings file
    for (int i = 0;settings_array[i];i++) {
        char** curr_elem_split = str_split(settings_array[i], '=');
        // make sure every line has a name and value seperated by =
        if(!curr_elem_split[1] || curr_elem_split[2]) {
            printf("ERROR IN %s ON LINE %d\n", argv[1], i+1);
            return 1;
        }

        if(!strcmp(curr_elem_split[0], "WEBHOOK_URL")) {
            if(webhook_url) {
                printf("ERROR IN %s ON LINE %d REDEFINITION OF WEBHOOK_URL\n", argv[1], i+1);
                return 1;
            }
            webhook_url = curr_elem_split[1];
        } else if(!strcmp(curr_elem_split[0], "PING_ID")) {
            if(ping_id) {
                printf("ERROR IN %s ON LINE %d REDEFINITION OF PING_ID\n", argv[1], i+1);
                return 1;
            }
            ping_id = curr_elem_split[1];
        } else if(!strcmp(curr_elem_split[0], "SERVER")) {
            char** currServ = str_split(curr_elem_split[1], '@');
            server_t tmpServer;
            tmpServer.prevOnline = false;
            // double NOT is kind of cursed
            tmpServer.portSpecified = !!strchr(currServ[1], ':');
            tmpServer.name = currServ[0];

            // if a port has been specifed, split the ip
            // by : and save the left as ip and the right as
            // port converted to an int, otherwise, save the
            // ip and set port to 0
            if(tmpServer.portSpecified) {
                char** tmpServerIp = str_split(currServ[1], ':');
                tmpServer.ip = tmpServerIp[0];
                tmpServer.port = atoi(tmpServerIp[1]);
            } else {
                tmpServer.ip = currServ[1];
                tmpServer.port = 0;
            }


            servers[elems_in_servers_arr] = tmpServer;
            elems_in_servers_arr++;

            // make sure servers array isnt overflowing
            if(elems_in_servers_arr+1>=curr_sizeof_servers) {
                server_t* servers_old = servers;
                servers = realloc(servers, (sizeof(server_t)*5+curr_sizeof_servers));
                curr_sizeof_servers+=5;
                if(!servers) {
                    printf("ERROR COULD NOT ALLOCATE %ld BYTES\n", (sizeof(server_t)*5+curr_sizeof_servers));
                    return 1;
                }
                free(servers_old);
            }

        } else {
            printf("ERROR IN %s ON LINE %d UNEXPECTED TOKEN\n", argv[1], i+1);
        }

    }

    // main loop
    while(1) {
        for (int i = 0; i < elems_in_servers_arr; i++) {
            // printf("checking:\nip:%s\nname:%s\nport:%d\nportNum:%d\nalive:%d\n", servers[i].ip, servers[i].name, servers[i].portSpecified, servers[i].port, servers[i].prevOnline);

            bool pingSuccess = false;
            // attempt socket connect
            if (servers[i].portSpecified) {
                pingSuccess = check_ip_port(servers[i].ip, servers[i].port, 5);
            } else {
                pingSuccess = ping(servers[i].ip);
            }

            // server was online last time we checked but isnt now
            if (!pingSuccess && servers[i].prevOnline) {
                // printf("server %s died\n", servers[i].name);

                char* tmpMsgPtr = malloc(strlen(discordOfflineMsg)+200);
                sprintf(tmpMsgPtr, discordOfflineMsg, ping_id, servers[i].name, servers[i].ip, servers[i].port);
                send_discord_message(webhook_url, tmpMsgPtr);
                free(tmpMsgPtr);

            // server wasnt online last time we checked but is now
            } else if (pingSuccess && !servers[i].prevOnline) {
                // printf("server %s online\n", servers[i].name);

                char* tmpMsgPtr = malloc(strlen(discordOnlineMsg)+200);
                sprintf(tmpMsgPtr, discordOnlineMsg, ping_id, servers[i].name, servers[i].ip, servers[i].port);
                puts(tmpMsgPtr);
                send_discord_message(webhook_url, tmpMsgPtr);
                free(tmpMsgPtr);
            }

            servers[i].prevOnline = pingSuccess;
        }
        sleep(1);
    }

    free(settings_file_buf);
    free(servers);

    return 0;
}

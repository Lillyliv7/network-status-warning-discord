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

struct list_ip{
    bool dead;
    char *ip;
    char *host;
};
typedef struct list_ip list_ip;


int main(int argc, char *argv[]) {

    if (argc == 1) {
        printf("Error no IP list file specified\nUsage: disc-serv-stat-alert ./settings.txt\n");
        return 1;
    }

    FILE *fptr;
    fptr = fopen(argv[1], "r");
    if(fptr == NULL) {
        puts("invalid settings file");
        return 1;
    }

    fseek(fptr, 0L, SEEK_END);
    long filesize = ftell(fptr);

    char* settings_file_buf = malloc(filesize + 500);

    if (settings_file_buf == NULL) {
        printf("error allocating %ld byte buffer to read file", filesize+500);
        return 1;
    }

    rewind(fptr);

    size_t bytesRead = fread(settings_file_buf, 1, filesize, fptr);
    if (bytesRead != filesize) {
        perror("Error reading file");
        free(settings_file_buf);
        fclose(fptr);
        return 1;
    }

    fclose(fptr);

    puts(settings_file_buf);

    // settings file data is now in settings_file_buf


    // handle settings file
    // split settings array
    char **settings_array;

    settings_array = str_split(settings_file_buf, '~');

    // set discord values from settings
    char *webhook_url = settings_array[0];
    char *discord_id = settings_array[1];

    unsigned int ip_count = atoi(settings_array[4]);
    printf("%d\n", ip_count);

    bool dead = false;

    list_ip *ip_list = malloc(sizeof(list_ip) * ip_count);

    for (int i = 0; i < ip_count; i++) {
        ip_list[i].dead = false;
        char **ip_arr;
        ip_arr = str_split(settings_array[i+5], '@');
        printf("%s@%s\n", ip_arr[0], ip_arr[1]);
        ip_list[i].host = ip_arr[0];
        ip_list[i].ip = ip_arr[1];
    }

    for(;;) {
        for (int i = 0; i < ip_count; i++) {
            char *message = malloc(500);
            bool ping_succeeded = ping(ip_list[i].ip);

            if (ping_succeeded == false && ip_list[i].dead == false) {
                sprintf(message, settings_array[2], ip_list[i].host, ip_list[i].ip, settings_array[1]);
                send_discord_message(webhook_url, message);
                ip_list[i].dead = true;
            }
            
            else if (ping_succeeded == true && ip_list[i].dead == true) {
                sprintf(message, settings_array[3], ip_list[i].host, ip_list[i].ip, settings_array[1]);
                send_discord_message(webhook_url, message);
                ip_list[i].dead = false;
            }
            sleep(2);
        }
    }

    free(settings_file_buf);
    free(ip_list);

    return 0;
}

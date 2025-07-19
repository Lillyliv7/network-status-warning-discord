#include <settings_file.h>

#include <stdio.h>
#include <stdlib.h>

char* read_settings_file(char* filepath) {
    FILE *fptr;
    fptr = fopen(filepath, "r");
    if(fptr == NULL) {
        puts("invalid settings file");
        return NULL;
    }

    // get file size and allocate buffer
    fseek(fptr, 0L, SEEK_END);
    long filesize = ftell(fptr);

    char* settings_file_buf = malloc(filesize);

    if (settings_file_buf == NULL) {
        printf("error allocating %ld byte buffer to read file", filesize);
        return NULL;
    }
    rewind(fptr);

    // read settings file into buffer
    size_t bytesRead = fread(settings_file_buf, 1, filesize, fptr);
    if (bytesRead != filesize) {
        perror("Error reading file");
        free(settings_file_buf);
        fclose(fptr);
        return NULL;
    }

    fclose(fptr);

    puts(settings_file_buf);
    return settings_file_buf;
}

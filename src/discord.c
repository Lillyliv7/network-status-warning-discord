#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

void send_discord_message(const char *webhook_url, const char *message) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // JSON payload to be sent to Discord webhook
        char json_data[1024];
        snprintf(json_data, sizeof(json_data), "{\"content\": \"%s\"}", message);

        // Set the URL for the webhook
        curl_easy_setopt(curl, CURLOPT_URL, webhook_url);

        // Set the content type to application/json
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Set the JSON data as the POST data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

/* 
  1) gcc prog.c -o obj -I/home/idontknow/anaconda3/include -c
  2) gcc obj -L/home/idontknow/anaconda3/lib -lcurl -o prog
*/

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <curl/curl.h>

typedef struct {
    char* data;
    size_t next;
} html_head;

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *data) {
    // printf("in - ");
    html_head* html_data = (html_head*)data;
    memcpy(html_data->data + html_data->next, ptr, size * nmemb);
    html_data->next += size * nmemb;
    // printf("out\n");
    return size * nmemb;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Must be 1 argument.\n");
        return 1;
    }
    CURL *curl;
    CURLcode res;
 
    curl = curl_easy_init();
    if (curl) {
        html_head data = {
            .data = malloc(10 * 1024 * 1024),
            .next = 0
        };
        memset(data.data, '\0', 10 * 1024 * 1024);

        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
 
        int res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
            return 1;
        }
        // printf("performed\n");

        char* start = strstr(data.data, "<title>") + 7;
        char* end = strstr(data.data, "</title>") - 1;

        if (start == NULL || end == NULL) {
            printf("No title found.\n");
            return 1;
        }

        end[1] = '\0';
        printf("%s", start);
 
        curl_easy_cleanup(curl);
        free(data.data);
    }
    return 0;
}

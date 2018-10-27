#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

size_t write_data(void * ptr, size_t size, size_t nmemb, void * stream);

int main(void)
{
	const char * path = "/tmp/curl-test";
    const char * mode = "w";
    FILE * fp;
    CURL * curl;
    CURLcode res;

    fp = fopen(path, mode);
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
    res = curl_easy_perform(curl);

    fclose(fp);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    printf("res: %d\n", res);

	return 0;
}

size_t write_data(void * ptr, size_t size, size_t nmemb, void * stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);

    return written;
}

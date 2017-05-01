#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <malloc.h>
#include <nss.h>

#include <strings.h>

//#include <errno.h>

#include <unistd.h>
#include <curl/curl.h>



//#include "nss-json.h"
#include "cJSON.h"
#include "cJSON-utils.h"

#include "nss-json-impl.h"


#define CFGFILE "/etc/nss-json.conf"


// File based caching; avoids sending out the http request at all
#define CACHEFILE "/root/.cache/nss-json-cache.json"

// In case the URL request fails, create a file to indicate this
// Note: Delete the file if it does not belong to root, so an attacker can not lock others out by creating it
#define ERRORFILE "/tmp/.nss-json-error.swp"



// In memory cache interval, before loading from the file again
#define CACHE_INTERVAL_IN_SEC 10


// source: http://stackoverflow.com/questions/2329571/c-libcurl-get-output-into-a-string
struct string {
  char *ptr;
  size_t len;
};

void init_string(struct string *s) {
  s->len = 0;
  s->ptr = malloc(s->len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "malloc() failed\n");
    exit(EXIT_FAILURE);
  }
  s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
  size_t new_len = s->len + size*nmemb;
  s->ptr = realloc(s->ptr, new_len+1);
  if (s->ptr == NULL) {
    fprintf(stderr, "realloc() failed\n");
    exit(EXIT_FAILURE);
  }
  memcpy(s->ptr+s->len, ptr, size*nmemb);
  s->ptr[new_len] = '\0';
  s->len = new_len;

  return size*nmemb;
}


cJSON* performRequest(const char *url, const cJSON* requestJson, int includeRequestArg, int verbose, int useGet)
{
    cJSON* result = NULL;
    CURL *curl;
    struct string s;
    char *requestJsonStr;

    if(includeRequestArg) {
        requestJsonStr = cJSON_Print(requestJson);
    }


    
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        init_string(&s);

        curl_easy_setopt(curl, CURLOPT_VERBOSE, verbose);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

//  curl_easy_setopt(curl, CURLOPT_POST, 1);
//  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "foo=bar&foz=baz");

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        result = cJSON_Parse(s.ptr);
        free(s.ptr);
    }

    if(includeRequestArg) {
        free(requestJsonStr);
    }


    return result;
}


/**
 * Read all data from fp into a buffer
 * Allocs in increments of the buffer size
 *
 */
char* read_data(FILE *fp) {
    char buf[4096];
    int i = 0;
    int d;
    int resultLen = sizeof(buf);
    char *p_result = malloc(resultLen);
    char *p_tmp;
    
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        d = strlen(buf);

        if(i + d >= resultLen) {
            // Below does ceiling ; i + d + 1 -> include terminating \0 char
            resultLen = (1 + (((i + d + 1) - 1) / sizeof(buf))) * sizeof(buf);
            p_tmp = realloc(p_result, resultLen);
            if(p_tmp == NULL) {
                free(p_result);
                p_result = NULL;
                break; 
            }
            p_result = p_tmp;
        }

        strncpy(&p_result[i], buf, d);
        i += d;
    }

    if(p_result != NULL) {
        p_result[i] = '\0';
    }
    //printf("data: %s", p_result);

    return p_result;
}


/**
 * Execute a command and parse its output as JSON
 * Note: I would have preferred to parse the output of cmd into a provided empty json object, but such method did not exist in cJSON
 * Therefore this pointer to the pointer argument.
 *
 * @return The exit code of the process
 * @param outJson Address of the pointer which to set to the JSON object; NULL if failed to parse
 * @param cmd The command to execute
 */
int read_json_old(cJSON** outJson, const char * const cmd) {
    FILE *fp;
    char* data;
    int result;

    // TODO Find out if there is a way to always execute as root
    //setuid(0);
    //char foo[1024];
    //getlogin_r(foo, sizeof(foo));
    //int foo = geteuid();
    //printf("XUSER: %d\n", foo);

    fp = popen(cmd, "r");
    if (fp == NULL) {
        result = 1;
        //printf("Failed to run command\n");
    } else {

        data = read_data(fp);
        result = WEXITSTATUS(pclose(fp));
        
        //printf("Read Status / Response Buffer: %s --- %d / %s\n", cmd, result, data);
        *outJson = cJSON_Parse(data);
        //printf("json: %d\n", *outJson);

        free(data);
    }

    return result;
}



enum nss_status delegate(const cJSON * const requestJson, cJSON** responseJson) {
    enum nss_status result;
    int exitCode;
    char buf[4096];
    // Render the requestJson as a string and pass it to the script

    // TODO Escape single quotes...
    // TODO Allocate buffer dynamically - use asprintf
    //snprintf(buf, sizeof(buf), "%s '%s'", CFGFILE, rendered);
    //printf("Request Call: %s", buf);

    // get config options
    readconfig(CFGFILE);
    const char *url = getcfg("url");
    int includeRequestArg = strcasecmp(getcfg("includeRequestArg"), "true") == 0;
    int verbose = strcasecmp(getcfg("verbose"), "true") == 0;

    *responseJson = performRequest(url, requestJson, includeRequestArg, verbose, 0);
    result = responseJson == NULL ? NSS_STATUS_UNAVAIL : NSS_STATUS_SUCCESS;

    cleanupconfig();
    
    return result;
}


enum nss_status delegate_old(const cJSON * const requestJson, cJSON** responseJson) {
    enum nss_status result;
    int exitCode;
    char buf[4096];
    // Render the requestJson as a string and pass it to the script
    char *rendered = cJSON_Print(requestJson);

    // TODO Escape single quotes...
    // TODO Allocate buffer dynamically - use asprintf
    snprintf(buf, sizeof(buf), "%s '%s'", CFGFILE, rendered);
    free(rendered);
    //printf("Request Call: %s", buf);

    exitCode = read_json_old(responseJson, buf);

    


    result = exitCode == 0 ? NSS_STATUS_SUCCESS : NSS_STATUS_UNAVAIL;

    // rendered = cJSON_Print(*responseJson);
    // printf("ResponseJson: %s", rendered);
    // free(rendered);

    return result;
}


static int firstRun = 1;
static clock_t before;
static cJSON* jsonCache = NULL;
static enum nss_status retvalCache = 0;

enum nss_status _nss_json_handle_request(const cJSON * const requestJson, cJSON** responseJson) {

    clock_t now = clock();
    clock_t elapsed;
    if(firstRun) {
        elapsed = CACHE_INTERVAL_IN_SEC;
        firstRun = 0;
    } else {
        elapsed = (double)(now - before) / CLOCKS_PER_SEC;
    }

    if(elapsed >= CACHE_INTERVAL_IN_SEC) {
        // printf("RECACHING");
        cJSON_Delete(jsonCache);
        jsonCache = NULL;
        retvalCache = delegate(requestJson, &jsonCache);
        before = clock();
    }

    *responseJson = cJSON_Duplicate(jsonCache, 1);

    return  retvalCache;
}



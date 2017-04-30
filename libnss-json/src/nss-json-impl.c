#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <malloc.h>
#include <nss.h>
//#include <errno.h>

//#include "nss-json.h"
#include "cJSON.h"
#include "cJSON-utils.h"


#define CFGFILE "/etc/nss-json"




/**
 * Read all data from fp into a buffer
 *
 * It does unnecessary *allocs due to simplicity. Feel free to replace or improve.
 */
char* read_data(FILE *fp) {
    char buf[1024];
    int n = 0;
    char *p_result = malloc(1);
    p_result[0] = '\0';

    while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
        p_result = realloc(p_result, n + sizeof(buf));

        strncpy(&p_result[n], buf, sizeof(buf) - 1);

        n += sizeof(buf) - 1;
    }

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
int read_json(cJSON** outJson, const char * const cmd) {
    FILE *fp;
    char* data;
    int result;

    fp = popen(cmd, "r");
    if (fp == NULL) {
        result = 1;
        //printf("Failed to run command\n");
        //exit(1);
    } else {

        data = read_data(fp);
        result = WEXITSTATUS(pclose(fp));
        
        //printf("Read Status / Response Buffer: %d / %s\n", result, data);
        *outJson = cJSON_Parse(data);
        //printf("json: %d\n", *outJson);

        free(data);
    }

    return result;
}


enum nss_status _nss_json_handle_request(const cJSON * const requestJson, cJSON** responseJson) {
    enum nss_status result;
    int exitCode;
    // Render the requestJson as a string and pass it to the script
    char *rendered = cJSON_Print(requestJson);

//    printf("RequestJson: %s", rendered);

    char buf[2048];
    // TODO Escape single quotes...
    // TODO Allocate buffer dynamically - use asprintf
    snprintf(buf, sizeof(buf), "%s '%s'", CFGFILE, rendered);
    free(rendered);
    //printf("Request Call: %s", buf);

    exitCode = read_json(responseJson, buf);
    result = exitCode == 0 ? NSS_STATUS_SUCCESS : NSS_STATUS_UNAVAIL;

//    rendered = cJSON_Print(*responseJson);
//    printf("ResponseJson: %s", rendered);
//    free(rendered);


    return result;
}


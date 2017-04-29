#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <malloc.h>
//#include <errno.h>

//#include "nss-json.h"
#include "cJSON.h"
#include "cJSON-utils.h"


#define CFGFILE "/etc/nss-json"




/**
 * Read all data from fp into a buffer
 *
 * It does unnecessary *allocs but can be read by humans. Feel free to replace or improve.
 */
char* read_data(FILE *fp) {
    char buf[1024];
    int n = 0;
    char *p_result = malloc(n);

    while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
        p_result = realloc(p_result, n + sizeof(buf));

        memcpy(&p_result[n], buf, sizeof(buf));

        n += sizeof(buf);
    }

    return p_result;
}


/**
 * Execute a command and parse its output as JSON
 * Note: I would have preferred to parse the output of cmd into a provided empty json object, but such method did not exist in cJSON
 * Therefore this pointer to the pointer argument.
 *
 * @return The exit code of the process
 * @param outJson Address of the pointer which to set to the JSON object
 * @param cmd The command to execute
 */
int read_json(cJSON** outJson, const char * const cmd) {
    FILE *fp;
    char* data;
    int result;

    fp = popen(cmd, "r");
    if (fp == NULL) {
        result = 1;
        //printf("Failed to run command\n" );
        //exit(1);
    } else {

      data = read_data(fp);
      result = WEXITSTATUS(pclose(fp));
      *outJson = cJSON_Parse(data);
      free(data);
    }

    return result;
}


int _nss_json_handle_request(const cJSON * const requestJson, cJSON** responseJson) {

    // Render the requestJson as a string and pass it to the script
    char *rendered = cJSON_Print(requestJson);

    char buf[2048];
    // TODO Escape single quotes...
    snprintf(buf, sizeof(buf), "%s '%s'", CFGFILE, rendered);

    int result = read_json(responseJson, buf);

    return result;
}


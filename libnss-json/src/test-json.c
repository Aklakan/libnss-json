#include <stdio.h>
#include <malloc.h>

#include "cJSON.h"
#include "cJSON-utils.h"
#include "nss-json-bindings.h"


// from nss-json-impl.c
char* read_data(FILE *fp);

int main(int argc, char *argv[] )
{
    cJSON* json;

    int processExitCode = read_json(&json, "/home/raven/Projects/Eclipse/sparql-auth/libnss-sparql/test.sh");
    printf("ExitCode: %d\n", processExitCode);
    char *rendered = cJSON_Print(json);
    printf("Parsed json: \n");
    printf("%s\n", rendered);
    printf("---\n");

    char buffer[4096];
    struct passwd *x = malloc(sizeof(struct passwd));
    json2pwd(json, x, buffer, sizeof(buffer), NULL);

    cJSON *y;
    y = cJSON_CreateObject();
    pwd2json(x, y);

    char *roundtripStr = cJSON_Print(y);
    printf("rountrip json:\n");
    printf("%s\n", roundtripStr);

    return 0;
}


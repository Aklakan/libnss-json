#include <stdio.h>
#include <malloc.h>

#include "cJSON.h"
#include "cJSON-utils.h"
#include "nss-json-bindings.h"

#include "nss-json.h"

// declaration for nss-json-impl.c
int read_json(cJSON** outJson, const char * const cmd);


int test_getpwnam(const char * const name)
{
    int result = 0;
    int errnop;
    char buffer[4096];
    struct passwd x;
    cJSON* json = cJSON_CreateObject();
    enum nss_status status = _nss_json_getpwnam_r(name, &x, buffer, sizeof(buffer), &errnop);

    if(status == NSS_STATUS_SUCCESS) {
        pwd2json(&x, json);
        cJSON_PrintPreallocated(json, buffer, sizeof(buffer), 0);
        printf("Data at test result: %s\n", buffer);
        cJSON_Delete(json);
    } else {
        result = -1;
    }

    return result;
}

int test_getpwuid(int uid)
{
    int result = 0;
    int errnop;
    char buffer[4096];
    struct passwd x;
    cJSON* json = cJSON_CreateObject();
    enum nss_status status = _nss_json_getpwuid_r(uid, &x, buffer, sizeof(buffer), &errnop);

    if(status == NSS_STATUS_SUCCESS) {
        pwd2json(&x, json);
        cJSON_PrintPreallocated(json, buffer, sizeof(buffer), 0);
        printf("Data at test result: %s\n", buffer);
        cJSON_Delete(json);
    } else {
        result = -1;
    }

    return result;
}


int test_getgrnam(const char * const name)
{
    int result = 0;
    int errnop;
    char buffer[4096];
    struct group x;
    cJSON* json = cJSON_CreateObject();
    enum nss_status status = _nss_json_getgrnam_r(name, &x, buffer, sizeof(buffer), &errnop);		

    if(status == NSS_STATUS_SUCCESS) {
        grp2json(&x, json);
        cJSON_PrintPreallocated(json, buffer, sizeof(buffer), 0);
        printf("Data at test result: %s\n", buffer);
        cJSON_Delete(json);
    } else {
        result = -1;
    }

    return result;
}


int test_getgrgid(int gid)
{
    int result = 0;
    int errnop;
    char buffer[4096];
    struct group x;
    cJSON* json = cJSON_CreateObject();
    enum nss_status status = _nss_json_getgrgid_r(gid, &x, buffer, sizeof(buffer), &errnop);		

    if(status == NSS_STATUS_SUCCESS) {
        grp2json(&x, json);
        cJSON_PrintPreallocated(json, buffer, sizeof(buffer), 0);
        printf("Data at test result: %s\n", buffer);
        cJSON_Delete(json);
    } else {
        result = -1;
    }

    return result;
}



int testRoundTrip1()
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

    cJSON *y = cJSON_CreateObject();
    pwd2json(x, y);

    char *roundtripStr = cJSON_Print(y);
    printf("rountrip json:\n");
    printf("%s\n", roundtripStr);

    return 0;
}


int performTestRun() {
    test_getpwnam("yoda");
    test_getpwuid(10000);

    test_getgrnam("yoda");
    test_getgrgid(10000);

    // Lookups with non-existing things should not fail either
    test_getpwnam("invisiblepinkunicorn");
    test_getpwuid(666666);

    test_getgrnam("invisiblepinkunicorn");
    test_getgrgid(666666);

    return 0;
}

int testForMemoryLeaks() {
    // Endless loop for detecting memory leaks
    for(;;) {
        performTestRun();
    }
}

int main(int argc, char *argv[]) {
    testForMemoryLeaks();

    return 0;
}


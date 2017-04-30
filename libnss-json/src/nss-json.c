#include <stdio.h>
#include <string.h>
#include <stdarg.h>


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <nss.h>

#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <sys/types.h>

#include "cJSON.h"
#include "cJSON-utils.h"
#include "nss-json-bindings.h"
#include "nss-json.h"
#include "nss-json-impl.h"


#define NSS_REQUEST  "request"

#define NSS_GETPWENT "getpwent"
#define NSS_GETGRENT "getgrent"

#define NSS_GETPWNAM "getpwnam"
#define NSS_GETPWUID "getpwuid"
#define NSS_PWUID    "uid"
#define NSS_PWNAM    "name"


#define NSS_GETGRNAM "getgrnam"
#define NSS_GETGRGID "getgrgid"
#define NSS_GRGID    "gid"
#define NSS_GRNAM    "name"

#define NSS_GRMEM    "members"


void cleanup(cJSON* requestJson, cJSON* resultJson) {
    cJSON_Delete(requestJson);
    if(resultJson != NULL) {
        cJSON_Delete(resultJson);
    }
}

// Delete non-NULL pointer to JSON and set to NULL
void _cJSON_SafeDelete(cJSON** p) {
    if(p != NULL && *p != NULL) {
        cJSON_Delete(*p);
        *p = NULL;
    }
}


static cJSON *openResponse = NULL;
static cJSON *openMatches = NULL;
static cJSON *cursor = NULL;

//        char x[1000];
//        cJSON_PrintPreallocated(openMatches, x, sizeof(x), 0);
//        printf("entry: %s\n", x);

enum nss_status _nss_json_setpwent(void)
{
    enum nss_status result;
    cJSON* requestJson = cJSON_CreateObject();
    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETPWENT));

    _cJSON_SafeDelete(&openResponse);
    result = _nss_json_handle_request(requestJson, &openResponse);
    cJSON_Delete(requestJson);

    if(result == NSS_STATUS_SUCCESS) {
        openMatches = _cJSON_GetStringMatches(openResponse, NSS_PWNAM, NULL, NSS_PWUID);
        cursor = openMatches->child;
    }

    return result;
}

enum nss_status _nss_json_endpwent(void)
{
    _cJSON_SafeDelete(&openResponse);
    _cJSON_SafeDelete(&openMatches);
    return NSS_STATUS_SUCCESS ;
}

enum nss_status _nss_json_getpwent_r(struct passwd *out, char *buffer, size_t buflen, int *errnop)
{
    enum nss_status result;
    if(cursor == NULL) {
        result = NSS_STATUS_NOTFOUND;
    } else {
        json2pwd(cursor, out, buffer, buflen, errnop);
        cursor = cursor->next;
        result = NSS_STATUS_SUCCESS;
    }
    return result;
}


int _nss_json_getpwnam_r (const char *name, struct passwd *out, char *buffer, size_t buflen, int *errnop)
{
    enum nss_status result;
    cJSON* requestJson = cJSON_CreateObject();
    cJSON* responseJson = NULL;

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETPWNAM));
    cJSON_AddItemToObject(requestJson, NSS_PWNAM, _cJSON_CreateStringOrNull(name));

    result = _nss_json_handle_request(requestJson, &responseJson);

    if(result == NSS_STATUS_SUCCESS) {
        cJSON* arr = _cJSON_GetStringMatches(responseJson, NSS_PWNAM, name, NSS_PWUID);
        cJSON* match = _cJSON_ExtractSingleResult(arr);

        if(match != NULL) {
            json2pwd(match, out, buffer, buflen, errnop);
        } else {
            result = NSS_STATUS_NOTFOUND;
        }
    }

    cleanup(requestJson, responseJson);

    return result;
}

int _nss_json_getpwuid_r (uid_t uid, struct passwd *out, char *buffer, size_t buflen, int *errnop)
{
    enum nss_status result;
    cJSON* responseJson = NULL;
    cJSON* requestJson = cJSON_CreateObject();

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETPWUID));
    cJSON_AddNumberToObject(requestJson, NSS_PWUID, uid);

    result = _nss_json_handle_request(requestJson, &responseJson);

    if(result == NSS_STATUS_SUCCESS) {
        cJSON* arr = _cJSON_GetIntMatches(responseJson, NSS_PWUID, uid, NSS_PWNAM);
        cJSON* match = _cJSON_ExtractSingleResult(arr);

        if(match != NULL) {
            json2pwd(match, out, buffer, buflen, errnop);
        } else {
            result = NSS_STATUS_NOTFOUND;
        }
    }

    cleanup(requestJson, responseJson);

    return result;
}







enum nss_status _nss_json_setgrent(void)
{
    enum nss_status result;
    cJSON* requestJson = cJSON_CreateObject();
    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETGRENT));

    _cJSON_SafeDelete(&openResponse);
    result = _nss_json_handle_request(requestJson, &openResponse);
    cJSON_Delete(requestJson);

    if(result == NSS_STATUS_SUCCESS) {
        openMatches = _cJSON_GetStringMatches(openResponse, NSS_GRMEM, NULL, NSS_GRGID);
        cursor = openMatches->child;
    }

    return result;
}

enum nss_status _nss_json_endgrent(void)
{
    _cJSON_SafeDelete(&openResponse);
    _cJSON_SafeDelete(&openMatches);
    return NSS_STATUS_SUCCESS ;
}

enum nss_status _nss_json_getgrent_r(struct group *out, char *buffer, size_t buflen, int *errnop)
{
    enum nss_status result;
    if(cursor == NULL) {
        result = NSS_STATUS_NOTFOUND;
    } else {
        json2grp(cursor, out, buffer, buflen, errnop);
        cursor = cursor->next;
        result = NSS_STATUS_SUCCESS;
    }
    return result;
}



enum nss_status _nss_json_getgrnam_r(const char *grnam, struct group *out,
                      char *buffer, size_t buflen, int *errnop)
{
    enum nss_status result;
    cJSON* responseJson = NULL;
    cJSON* requestJson = cJSON_CreateObject();

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETGRNAM));
    cJSON_AddItemToObject(requestJson, NSS_GRNAM, _cJSON_CreateStringOrNull(grnam));

    result = _nss_json_handle_request(requestJson, &responseJson);

    if(result == NSS_STATUS_SUCCESS) {
        cJSON* arr = _cJSON_GetStringMatches(responseJson, NSS_GRNAM, grnam, NSS_GRMEM);
//printf("At %s: %d matches for %s %s %s\n", __FUNCTION__, cJSON_GetArraySize(arr), NSS_GRNAM, grnam, NSS_GRMEM);
        cJSON* match = _cJSON_ExtractSingleResult(arr);

        if(match != NULL) {
            json2grp(match, out, buffer, buflen, errnop);
        } else {
            result = NSS_STATUS_NOTFOUND;
        }
    }

    cleanup(requestJson, responseJson);

    return result;
}

enum nss_status _nss_json_getgrgid_r(uid_t gid, struct group *out,
                      char *buffer, size_t buflen, int *errnop)
{
    enum nss_status result;
    cJSON* responseJson = NULL;
    cJSON* requestJson = cJSON_CreateObject();

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETGRGID));
    cJSON_AddNumberToObject(requestJson, NSS_GRGID, gid);

    result = _nss_json_handle_request(requestJson, &responseJson);

    if(result == NSS_STATUS_SUCCESS) {
        cJSON* arr = _cJSON_GetIntMatches(responseJson, NSS_GRGID, gid, NSS_GRMEM);
        cJSON* match = _cJSON_ExtractSingleResult(arr);

        if(match != NULL) {
            json2grp(match, out, buffer, buflen, errnop);
        } else {
            result = NSS_STATUS_NOTFOUND;
        }
    }

    cleanup(requestJson, responseJson);

    return result;
}




/*
enum nss_status
_nss_json_getpwbynam (const char *nam, struct passwd *result, char *buffer,
   size_t buflen, int *errnop) {
 printf( "@ %s with name %s\n", __FUNCTION__, nam ) ;
 return init_result(nam, result, buffer, buflen, errnop ) ;
};

enum nss_status
_nss_json_getpwbynam_r (const char *nam, struct passwd *result, char *buffer,
   size_t buflen, int *errnop) {
 printf( "@ %s with name_r %s\n", __FUNCTION__, nam ) ;
 return init_result(nam, result, buffer, buflen, errnop ) ;
};

*/

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


#define NSS_REQUEST  "request"

#define NSS_GETPWNAM "getpwnam"
#define NSS_GETPWUID "getpwuid"
#define NSS_PWUID    "uid"
#define NSS_PWNAM    "name"


#define NSS_GETGRNAM "getgrnam"
#define NSS_GETGRGID "getgrgid"
#define NSS_GRGID    "gid"
#define NSS_GRNAM    "name"


void cleanup(cJSON* requestJson, cJSON* resultJson) {
    cJSON_Delete(requestJson);
    if(resultJson != NULL) {
        cJSON_Delete(resultJson);
    }
}


int init_result(const char *nam, struct passwd *result, char *buffer,
   size_t buflen, int *errnop) {
    return NSS_STATUS_SUCCESS;
}


enum nss_status _nss_json_setpwent (void)
{
    //printf( "@ %s\n", __FUNCTION__ ) ;
    return NSS_STATUS_SUCCESS ;
}

enum nss_status _nss_json_endpwent (void)
{
     //printf( "@ %s\n", __FUNCTION__ ) ;
    return NSS_STATUS_SUCCESS ;
}

enum nss_status _nss_json_getpwent_r (struct passwd *result, char *buffer, size_t buflen, int *errnop)
{
    //printf( "@ %s\n", __FUNCTION__ ) ;
    return NSS_STATUS_NOTFOUND;
}

/*
 static int i = 0 ;

 if( i++ == 0 ) {
   printf( "@ %s\n", __FUNCTION__ ) ;
   return init_result(0, result, buffer, buflen, errnop ) ;
 } else {
   i = 0 ;
   return NSS_STATUS_NOTFOUND ;
 }
};
*/

int _nss_json_getpwnam_r (const char *name, struct passwd *out, char *buffer, size_t buflen, int *errnop)
{
    int result;
    cJSON* resultJson = NULL;
    cJSON* requestJson = cJSON_CreateObject();

    printf( "@ %s\n", __FUNCTION__ ) ;

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETPWNAM));
    cJSON_AddItemToObject(requestJson, NSS_PWNAM, _cJSON_CreateStringOrNull(name));

    result = _nss_json_handle_request(requestJson, &resultJson);

    if(result == NSS_STATUS_SUCCESS) {
        json2pwd(resultJson, out, buffer, buflen, errnop);
    }

    cleanup(requestJson, resultJson);

    // TODO We could validate whether the result matches the requested user id

    return result;
}

int _nss_json_getpwuid_r (uid_t uid, struct passwd *out, char *buffer, size_t buflen, int *errnop)
{
    int result;
    cJSON* resultJson = NULL;
    cJSON* requestJson = cJSON_CreateObject();

    printf( "@ %s\n", __FUNCTION__ ) ;

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETPWUID));
    cJSON_AddNumberToObject(requestJson, NSS_PWUID, uid);

    result = _nss_json_handle_request(requestJson, &resultJson);

    if(result == NSS_STATUS_SUCCESS) {
        json2pwd(resultJson, out, buffer, buflen, errnop);
    }

    cleanup(requestJson, resultJson);

    // TODO We could validate whether the result matches the requested user id

    return result;
}



enum nss_status
_nss_json_getgrnam_r(const char *grnam, struct group *out,
                      char *buffer, size_t buflen, int *errnop)
{
    int result;
    cJSON* resultJson = NULL;
    cJSON* requestJson = cJSON_CreateObject();

    printf( "@ %s\n", __FUNCTION__ ) ;

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETGRNAM));
    cJSON_AddItemToObject(requestJson, NSS_GRGID, _cJSON_CreateStringOrNull(grnam));

    result = _nss_json_handle_request(requestJson, &resultJson);

    if(result == NSS_STATUS_SUCCESS) {
        json2grp(resultJson, out, buffer, buflen, errnop);
    }

    cleanup(requestJson, resultJson);

    // TODO We could validate whether the result matches the requested user id

    return result;
}

enum nss_status
_nss_json_getgrgid_r(uid_t gid, struct group *out,
                      char *buffer, size_t buflen, int *errnop)
{
    int result;
    cJSON* resultJson = NULL;
    cJSON* requestJson = cJSON_CreateObject();

    printf( "@ %s\n", __FUNCTION__ ) ;

    cJSON_AddItemToObject(requestJson, NSS_REQUEST, _cJSON_CreateStringOrNull(NSS_GETGRGID));
    cJSON_AddNumberToObject(requestJson, NSS_GRGID, gid);

    result = _nss_json_handle_request(requestJson, &resultJson);

    if(result == NSS_STATUS_SUCCESS) {
        json2grp(resultJson, out, buffer, buflen, errnop);
    }

    cleanup(requestJson, resultJson);

    // TODO We could validate whether the result matches the requested user id

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


#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <nss.h>
#include <errno.h>
#include <sys/types.h>

#include "cJSON.h"
#include "cJSON-utils.h"
#include "nss-json-bindings.h"

/**
 * I quote:
 * "With apologies to nss_ldap..." ~ backend.c of nss_pgsql
 * and add:
 * "... and nss_pgsql"
 */

// TODO Maybe prefix fields, so we can have just a single JSON-LD context for passwd, group and shadow without running into problems due to overlapping field names.

/* define group column names */
#define GROUP_NAME    "name"
#define GROUP_PASSWD  "passwd"
#define GROUP_GID     "gid"
#define GROUP_GRMEM   "members"
/* define passwd column names */
#define PASSWD_NAME   "name"
#define PASSWD_PASSWD "passwd"
#define PASSWD_UID    "uid"
#define PASSWD_GID    "gid"
#define PASSWD_GECOS  "gecos"
#define PASSWD_DIR    "dir"
#define PASSWD_SHELL  "shell"
/* define shadow column names */
#define SHADOW_NAME   "name"
#define SHADOW_PASSWD "passwd"
#define SHADOW_LSTCHG "lstchg"
#define SHADOW_MIN    "min"
#define SHADOW_MAX    "max"
#define SHADOW_WARN   "warn"
#define SHADOW_INACT  "inact"
#define SHADOW_EXPIRE "expire"
#define SHADOW_FLAG   "flag"




/**
 *
 * @param sptr The input string
 * @param valptr A pointer to sptr's copy within the buffer
 */
enum nss_status copy_nss_string(const char *sptr, char **valptr, char **buffer, size_t *buflen, int *errnop)
{
    size_t slen;

    // If the input is NULL, set the output pointer to NULL and leave the rest unchanged
    if(sptr == NULL) {
        *valptr = NULL;
    } else {

        slen = strlen(sptr);
        if(*buflen < slen+1) {
            if(errnop != NULL) {
                *errnop = ERANGE;
            }
            return NSS_STATUS_TRYAGAIN;
        }
        strncpy(*buffer, sptr, slen);
        (*buffer)[slen] = '\0';
                
        *valptr = *buffer;

        *buffer += slen + 1;
        *buflen -= slen + 1;
    }
    return NSS_STATUS_SUCCESS;
}

/*
enum nss_status copy_json_nss_string(
    const cJSON *json,
    const char *defaultValue, // Fallback if json is null
    char **valptr,
    char **buffer,
    size_t *buflen,
    int *errnop)
{
    int result;

    const char *str = cJSON_IsString(json) ? json->valuestring : defaultValue;

    result = copy_nss_string(str, valptr, buffer, buflen, errnop);
    return result;
}
*/


int pwd2json(struct passwd *in, cJSON *result) {

    cJSON_AddItemToObject(result, PASSWD_NAME, _cJSON_CreateStringOrNull(in->pw_name));
    cJSON_AddItemToObject(result, PASSWD_PASSWD, _cJSON_CreateStringOrNull(in->pw_passwd));

    cJSON_AddNumberToObject(result, PASSWD_UID, in->pw_uid);
    cJSON_AddNumberToObject(result, PASSWD_GID, in->pw_gid);

    cJSON_AddItemToObject(result, PASSWD_GECOS, _cJSON_CreateStringOrNull(in->pw_gecos));
    cJSON_AddItemToObject(result, PASSWD_DIR, _cJSON_CreateStringOrNull(in->pw_dir));
    cJSON_AddItemToObject(result, PASSWD_SHELL, _cJSON_CreateStringOrNull(in->pw_shell));

    return 0;
}

//https://www.gnu.org/software/libc/manual/html_node/NSS-Modules-Interface.html

enum nss_status json2pwd(cJSON *json, struct passwd *result,
                        char *buffer,
                        size_t buflen, int *errnop)
{
    enum nss_status status = NSS_STATUS_NOTFOUND;

    if(json == NULL) {
        goto BAIL_OUT;
    }

    // Carefully copy attributes into buffer.  Return NSS_STATUS_TRYAGAIN if not enough room.
    // Must return passwd_name, passwd_passwd, passwd_gecos, passwd_dir, passwd_shell, passwd_uid, passwd_gid
    status = copy_nss_string(_cJSON_GetStringByKey(json, PASSWD_NAME, NULL), &result->pw_name, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    status = copy_nss_string(_cJSON_GetStringByKey(json, PASSWD_PASSWD, NULL), &result->pw_passwd, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    status = copy_nss_string(_cJSON_GetStringByKey(json, PASSWD_GECOS, NULL), &result->pw_gecos, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    status = copy_nss_string(_cJSON_GetStringByKey(json, PASSWD_DIR, NULL), &result->pw_dir, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    status = copy_nss_string(_cJSON_GetStringByKey(json, PASSWD_SHELL, NULL), &result->pw_shell, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    // Can be less careful with uid/gid
    // Fallback to 'nobody' if no id present
    result->pw_uid = _cJSON_GetIntByKey(json, PASSWD_UID, 65534);
    result->pw_gid = _cJSON_GetIntByKey(json, PASSWD_GID, 65534);

BAIL_OUT:
    return status;
}




int grp2json(struct group *in, cJSON *result) {

    char** p = in->gr_mem;
    cJSON* members = cJSON_CreateArray();

    cJSON_AddItemToObject(result, GROUP_NAME, _cJSON_CreateStringOrNull(in->gr_name));
    cJSON_AddItemToObject(result, GROUP_PASSWD, _cJSON_CreateStringOrNull(in->gr_passwd));
    cJSON_AddNumberToObject(result, GROUP_GID, in->gr_gid);

    // Append all members
    cJSON_AddItemToObject(result, GROUP_GRMEM, members);

    if(p != NULL) {
        while(*p != NULL) {
            cJSON_AddItemToArray(members, cJSON_CreateString(*p));
            ++p;
        }
    }

    return 0;
}



/*
 * return array of strings containing usernames that are member of group with gid 'gid'
 */
enum nss_status getgroupmem(cJSON *json,
                            struct group *result,
                            char *buffer,
                            size_t buflen, int *errnop)
{
    cJSON *item;
    int n, t = 0;
    enum nss_status status = NSS_STATUS_NOTFOUND;
    size_t ptrsize;

/*
    params[0] = malloc(12);
    n = snprintf(params[0], 12, "%d", gid);
    if (n == -1 || n > 12) {
        status = NSS_STATUS_UNAVAIL;
        *errnop = EAGAIN;
        goto BAIL_OUT;
    }
*/

	//res = PQexecParams(_conn, getcfg("getgroupmembersbygid"), 1, NULL, (const char**)params, NULL, NULL, 0);
/*
	if(PQresultStatus(res) != PGRES_TUPLES_OK) {
		status = NSS_STATUS_UNAVAIL;
		goto BAIL_OUT;
	}
*/

    if(!cJSON_IsArray(json))
        goto BAIL_OUT;

    n = cJSON_GetArraySize(json);

    // Make sure there's enough room for the array of pointers to group member names
    ptrsize = (n+1) * sizeof(const char *);
    if(buflen < ptrsize) {
        status = NSS_STATUS_TRYAGAIN;
        *errnop = ERANGE;
        goto BAIL_OUT;
    }

    /* realign the buffer on a 4-byte boundary */
    buflen -= 4-((long)buffer & 0x3);
    buffer += 4-((long)buffer & 0x3);

    result->gr_mem = (char**)buffer;

    buffer += (ptrsize+3)&(~0x3);
    buflen -= (ptrsize+3)&(~0x3);

//	for(t = 0; t < n; t++) {
    cJSON_ArrayForEach(item, json) {
        status = copy_nss_string(item->valuestring, &(result->gr_mem[t]), &buffer, &buflen, errnop);
        if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;
        ++t;
    }

    // If the group has no members, there was still success
    if (n == 0) {
        *errnop = 0;
        status = NSS_STATUS_SUCCESS;
    }
    result->gr_mem[n] = NULL;
	
BAIL_OUT:
    return status;
}


enum nss_status json2grp(cJSON *json, struct group *result,
                        char *buffer,
                        size_t buflen, int *errnop)
{
    enum nss_status status = NSS_STATUS_NOTFOUND;
    cJSON *members = cJSON_GetObjectItem(json, GROUP_GRMEM);

    if(json == NULL) {
        goto BAIL_OUT;
    }

    // Carefully copy attributes into buffer.  Return NSS_STATUS_TRYAGAIN if not enough room.
    // Must return passwd_name, passwd_passwd, passwd_gecos, passwd_dir, passwd_shell, passwd_uid, passwd_gid
    status = copy_nss_string(_cJSON_GetStringByKey(json, GROUP_NAME, NULL), &result->gr_name, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    status = copy_nss_string(_cJSON_GetStringByKey(json, GROUP_PASSWD, NULL), &result->gr_passwd, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    // Can be less careful with uid/gid
    // Fallback to 'nobody' if no id present
    result->gr_gid = _cJSON_GetIntByKey(json, GROUP_GID, 65534);

    
    status = getgroupmem(members, result, buffer, buflen, errnop);

BAIL_OUT:
    return status;
}




int shadow2json(struct spwd *in, cJSON *result) {

    cJSON_AddItemToObject(result, GROUP_NAME, _cJSON_CreateStringOrNull(in->sp_namp));
    cJSON_AddItemToObject(result, GROUP_PASSWD, _cJSON_CreateStringOrNull(in->sp_pwdp));

    cJSON_AddNumberToObject(result, SHADOW_LSTCHG, in->sp_lstchg);
    cJSON_AddNumberToObject(result, SHADOW_MIN, in->sp_min);
    cJSON_AddNumberToObject(result, SHADOW_MAX, in->sp_max);
    cJSON_AddNumberToObject(result, SHADOW_WARN, in->sp_warn);
    cJSON_AddNumberToObject(result, SHADOW_INACT, in->sp_inact);
    cJSON_AddNumberToObject(result, SHADOW_EXPIRE, in->sp_expire);
    cJSON_AddNumberToObject(result, SHADOW_FLAG, in->sp_flag);

    return 0;
}

/*
 * 'convert' a PGresult to struct shadow
 */
enum nss_status json2shadow(cJSON *json, struct spwd *result,
                        char *buffer,
                        size_t buflen, int *errnop)
{
    enum nss_status status = NSS_STATUS_NOTFOUND;

    if(json == NULL) {
        goto BAIL_OUT;
    }

    // Carefully copy attributes into buffer.  Return NSS_STATUS_TRYAGAIN if not enough room.
    status = copy_nss_string(_cJSON_GetStringByKey(json, SHADOW_NAME, NULL), &result->sp_namp, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    status = copy_nss_string(_cJSON_GetStringByKey(json, SHADOW_PASSWD, NULL), &result->sp_pwdp, &buffer, &buflen, errnop);
    if(status != NSS_STATUS_SUCCESS) goto BAIL_OUT;

    // Can be less careful with int variables
    result->sp_lstchg = _cJSON_GetIntByKey(json, SHADOW_LSTCHG, 0);
    result->sp_min = _cJSON_GetIntByKey(json, SHADOW_MIN, 0);
    result->sp_max = _cJSON_GetIntByKey(json, SHADOW_MAX, 0);
    result->sp_warn = _cJSON_GetIntByKey(json, SHADOW_WARN, 0);
    result->sp_inact = _cJSON_GetIntByKey(json, SHADOW_INACT, 0);
    result->sp_expire = _cJSON_GetIntByKey(json, SHADOW_EXPIRE, 0);
    result->sp_flag = _cJSON_GetIntByKey(json, SHADOW_FLAG, 0);

BAIL_OUT:
    return status;
}



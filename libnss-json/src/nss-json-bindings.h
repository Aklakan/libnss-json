//#include "cJSON.h"
#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <nss.h>

struct cJSON;

enum nss_status copy_nss_string(const char *sptr, char **valptr, char **buffer, size_t *buflen, int *errnop);

int _cJSON_GetInt(const cJSON * const json, int defaultValue);
const char* _cJSON_GetString(const cJSON * json, const char* const defaultValue);


int _cJSON_GetIntByKey(const cJSON * const json, const char* const key, int defaultValue);
const char* _cJSON_GetStringByKey(const cJSON * const json, const char * const key, const char * const defaultValue);
cJSON* _cJSON_CreateStringOrNull(const char * const str);

int pwd2json(struct passwd *in, cJSON *result);

enum nss_status json2pwd(cJSON *json, struct passwd *result,
                        char *buffer,
                        size_t buflen, int *errnop);


int grp2json(struct group *in, cJSON *result);

enum nss_status json2grp(cJSON *json, struct group *result,
                        char *buffer,
                        size_t buflen, int *errnop);


int shadow2json(struct spwd *in, cJSON *result);

enum nss_status json2shadow(cJSON *json, struct spwd *result,
                        char *buffer,
                        size_t buflen, int *errnop);



//#include "cJSON.h"

#include <nss.h>

int _nss_json_getpwnam_r (const char *name, struct passwd *out, char *buffer, size_t buflen, int *errnop);

int _nss_json_getpwuid_r (uid_t uid, struct passwd *out, char *buffer, size_t buflen, int *errnop);


enum nss_status _nss_json_getgrnam_r(const char *grnam, struct group *out,
                      char *buffer, size_t buflen, int *errnop);

enum nss_status _nss_json_getgrgid_r(uid_t gid, struct group *out,
                      char *buffer, size_t buflen, int *errnop);


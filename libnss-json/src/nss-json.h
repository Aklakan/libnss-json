//#include "cJSON.h"

#include <nss.h>

enum nss_status _nss_json_setpwent (void);
enum nss_status _nss_json_endpwent (void);
enum nss_status _nss_json_getpwent_r (struct passwd *out, char *buffer, size_t buflen, int *errnop);


enum nss_status _nss_json_getpwnam_r (const char *name, struct passwd *out, char *buffer, size_t buflen, int *errnop);

enum nss_status _nss_json_getpwuid_r (uid_t uid, struct passwd *out, char *buffer, size_t buflen, int *errnop);



enum nss_status _nss_json_setgrent (void);
enum nss_status _nss_json_endgrent (void);
enum nss_status _nss_json_getgrent_r (struct group *out, char *buffer, size_t buflen, int *errnop);

enum nss_status _nss_json_getgrnam_r(const char *grnam, struct group *out,
                      char *buffer, size_t buflen, int *errnop);

enum nss_status _nss_json_getgrgid_r(uid_t gid, struct group *out,
                      char *buffer, size_t buflen, int *errnop);


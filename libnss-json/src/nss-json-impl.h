//#include "cJSON.h"
struct cJSON;

enum nss_status _nss_json_handle_request(const struct cJSON * const requestJson, struct cJSON** result);


int readconfig(char* configfile);
void cleanupconfig(void);
char *getcfg(const char *key);


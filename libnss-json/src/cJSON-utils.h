//#include "cJSON.h"

// Utility functions which I think should be provided by cJSON out of the box but aren't

struct cJSON;

int _cJSON_GetInt(const cJSON * const json, int defaultValue);

const char* _cJSON_GetString(const cJSON * json, const char* const defaultValue);

int _cJSON_GetIntByKey(const cJSON * const json, const char* const key, int defaultValue);

const char* _cJSON_GetStringByKey(const cJSON * const json, const char * const key, const char * const defaultValue);

cJSON* _cJSON_CreateStringOrNull(const char * const str);


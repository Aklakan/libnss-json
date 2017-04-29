#include "cJSON.h"
#include "cJSON-utils.h"

// Utility functions which I think should be provided by cJSON out of the box but aren't

int _cJSON_GetInt(const cJSON * const json, int defaultValue) {
    int result = cJSON_IsNumber(json) ? json->valueint : defaultValue;
    return result;
}

const char* _cJSON_GetString(const cJSON * json, const char* const defaultValue) {
    const char *result = cJSON_IsString(json) ? json->valuestring : defaultValue;
    return result;
}


int _cJSON_GetIntByKey(const cJSON * const json, const char* const key, int defaultValue) {
  const cJSON * const child = cJSON_GetObjectItem(json, key);
  int result = _cJSON_GetInt(child, defaultValue);
  return result;
}

const char* _cJSON_GetStringByKey(const cJSON * const json, const char * const key, const char * const defaultValue) {
  const cJSON * const child = cJSON_GetObjectItem(json, key);
  const char *result = _cJSON_GetString(child, defaultValue);
  return result;
}

cJSON* _cJSON_CreateStringOrNull(const char * const str) {
    cJSON* result = str == NULL ? cJSON_CreateNull() : cJSON_CreateString(str);
    return result;
}


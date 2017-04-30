//#include "cJSON.h"

// Utility functions which I think should be provided by cJSON out of the box but aren't

struct cJSON;

int _cJSON_GetInt(const cJSON * const json, int defaultValue);

const char* _cJSON_GetString(const cJSON * json, const char* const defaultValue);

int _cJSON_GetIntByKey(const cJSON * const json, const char* const key, int defaultValue);

const char* _cJSON_GetStringByKey(const cJSON * const json, const char * const key, const char * const defaultValue);

cJSON* _cJSON_CreateStringOrNull(const char * const str);



//cJSON* _cJSON_GetMatches(cJSON * json, int (*predicate)(const cJSON*));


// Requires key to be non-nully
int _cJSON_KeyExists(const cJSON * const json, const char * key);
int _cJSON_IsStringMatch(const cJSON * const json, const char * key, const char * value);
int _cJSON_IsIntMatch(const cJSON * const json, const char * key, int value);


/**
 * If we just had lambdas...
 * Methods to find array elements having a specific key-value pair and optionally a specific key.
 * A non-array json object is treated as an array having a single item
 *
 * Always returns a new json array containing references to the items that matched the given predicate.
 * 
 *
 */
cJSON* _cJSON_GetIntMatches(cJSON * json, const char * key, int value, const char * requiredKey);
cJSON* _cJSON_GetStringMatches(cJSON * json, const char * key, const char * value, const char * requiredKey);
cJSON* _cJSON_ExtractSingleResult(cJSON* arr);


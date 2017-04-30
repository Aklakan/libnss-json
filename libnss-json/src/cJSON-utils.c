#include "cJSON.h"
#include "cJSON-utils.h"
#include <string.h>

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


/**
 * Native functions that implement the nss lookups on an in-memory json array.
 * This allows the user script to be as dumb as to always return the same set of json objects for every request (e.g. read from a file);
 * and here we are so intelligent to find the response ourselves.
 */


// Requires key to be non-nully
int _cJSON_KeyExists(const cJSON * const json, const char * key) {
    
    int result;
    if(json == NULL) {
        result = 0;
    } else if(key == NULL) {
        result = 1;
    } else {
        const cJSON* attr = cJSON_GetObjectItem(json, key);
        result = attr != NULL && !cJSON_IsNull(attr);
    } 
//printf("Data at %s: %d for %s\n", __FUNCTION__, result, key);
    return result;
}

int _cJSON_IsStringMatch(const cJSON * const json, const char * key, const char * value) {
    const cJSON* attr = cJSON_GetObjectItem(json, key);
    // If no value is provided, only the key has to exist
    int result = (value == NULL) ? attr != NULL && !cJSON_IsNull(attr) : cJSON_IsString(attr) && strcmp(value, attr->valuestring) == 0;
//printf("Data at %s: %d for %s %s\n", __FUNCTION__, result, key, value);
    return result;
}

int _cJSON_IsIntMatch(const cJSON * const json, const char * key, int value) {
    cJSON* attr = cJSON_GetObjectItem(json, key);
    int result = cJSON_IsNumber(attr) && value == attr->valueint;
    return result;
}



cJSON* _cJSON_GetIntMatches(cJSON * json, const char * key, int value, const char * requiredKey)
{
    cJSON* result = cJSON_CreateArray();
    cJSON* item;

    if(cJSON_IsArray(json)) {
        cJSON_ArrayForEach(item, json) {
            if(_cJSON_IsIntMatch(item, key, value) && _cJSON_KeyExists(item, requiredKey)) {
                cJSON_AddItemReferenceToArray(result, item);
            }
        }
    } else if(cJSON_IsObject(json)) {
        item = json;
        if(_cJSON_IsIntMatch(item, key, value) && _cJSON_KeyExists(item, requiredKey)) {
            cJSON_AddItemReferenceToArray(result, item);
        }
    }    

    return result;
}


cJSON* _cJSON_GetStringMatches(cJSON * json, const char * key, const char * value, const char * requiredKey)
{
    cJSON* result = cJSON_CreateArray();
    cJSON* item;

    if(cJSON_IsArray(json)) {
        cJSON_ArrayForEach(item, json) {
            if(_cJSON_IsStringMatch(item, key, value) && _cJSON_KeyExists(item, requiredKey)) {
                cJSON_AddItemReferenceToArray(result, item);
            }
        }
    } else if(cJSON_IsObject(json)) {
        item = json;
        if(_cJSON_IsStringMatch(item, key, value) && _cJSON_KeyExists(item, requiredKey)) {
            cJSON_AddItemReferenceToArray(result, item);
        }
    }    

    return result;
}

/**
 * Argument is expected to be an array of references only!
 * Returns the single item in this array. If arr is empty or there are more items, returns NULL.
 * Always deletes the provided array.
 */
cJSON* _cJSON_ExtractSingleResult(cJSON* arr) {
    cJSON* tmp = cJSON_IsArray(arr) && cJSON_GetArraySize(arr) == 1 ? cJSON_GetArrayItem(arr, 0) : NULL;
    cJSON* result = tmp == NULL ? NULL : tmp->ref;

    cJSON_Delete(arr);
    return result;
}



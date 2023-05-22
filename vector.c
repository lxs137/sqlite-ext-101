/*
** This SQLite extension implements a tick() function used for testing.
**
** Variants:
**
**    vector_version()    print extension version
**    vector_to_blob(X)   echo X
**
*/

#include "vector.h"

#include <assert.h>

#include "utils.h"

SQLITE_EXTENSION_INIT3

static void vector_version(sqlite3_context *context, int argc,
                           sqlite3_value **argv) {
  sqlite3_result_text(context, SQLITE_EXT_VERSION, -1, SQLITE_STATIC);
}

static void vector_json_to_blob(sqlite3_context *context, int argc,
                                sqlite3_value **argv) {
  assert(argc == 1);
  Vector *vec = parse_vector(argv[0]);
  if (vec == NULL) {
    sqlite3_result_error(context, "invalid vector", -1);
    goto clean;
  }
  int sizeData;
  void *data = vector_to_blob(vec, &sizeData);
  if (data == NULL) {
    sqlite3_result_error(context, "empty vector", -1);
    goto clean;
  }
  sqlite3_result_blob(context, data, sizeData, sqlite3_free);
  goto clean;

clean:
  if (vec != NULL) vector_free(vec);
  return;
}

static void vector_blob_to_text(sqlite3_context *context, int argc,
                                sqlite3_value **argv) {
  assert(argc == 1);
  Vector *vec = parse_vector(argv[0]);
  if (vec == NULL) {
    sqlite3_result_error(context, "invalid vector", -1);
    goto clean;
  }
  char *vecBrief = sqlite3_mprintf("vector{ dim: %d }", vec->dim);
  sqlite3_result_text(context, vecBrief, strlen(vecBrief), sqlite3_free);
  goto clean;

clean:
  if (vec != NULL) vector_free(vec);
  return;
}

static void vector_json_to_text(sqlite3_context *context, int argc,
                                sqlite3_value **argv) {
  assert(argc == 1);
  Vector *vec = parse_vector(argv[0]);
  if (vec == NULL) {
    sqlite3_result_error(context, "invalid vector", -1);
    goto clean;
  }
  char *vecBrief = sqlite3_mprintf("vector{ dim: %d }", vec->dim);
  sqlite3_result_text(context, vecBrief, strlen(vecBrief), sqlite3_free);
  goto clean;

clean:
  if (vec != NULL) vector_free(vec);
  return;
}

Vector *parse_vector(sqlite3_value *value) {
  Vector *vec;

  switch (sqlite3_value_type(value)) {
    case SQLITE3_TEXT:
      return vector_from_json_text((const char *)sqlite3_value_text(value));
    case SQLITE_BLOB:
        // int n = sqlite3_value_bytes(value);
        // const void *b;
        return vector_from_blob(sqlite3_value_blob(value),
                                sqlite3_value_bytes(value));
    default:
      return NULL;
  }
}

Vector *vector_new(int dim) {
  Vector *vec = sqlite3_malloc(sizeof(Vector));
  vec->dim = dim;
  vec->nUsed = 0;
  vec->x = (float *)sqlite3_malloc(sizeof(float) * dim);
  return vec;
}

void vector_add_item(Vector *vec, float v) {
  if (vec->nUsed >= vec->dim) return;
  vec->x[vec->nUsed] = v;
  vec->nUsed++;
}

void *vector_to_blob(Vector *vec, int *size) {
  if (vec->nUsed <= 0) return NULL;
  size_t szF = sizeof(float);
  *size = sizeof(int) + vec->nUsed * szF;
  void *data = sqlite3_malloc(*size);
  memset(data, 0, *size);
  memcpy((void *)((char *)data), (void *)&vec->nUsed, sizeof(int));
  memcpy((void *)((char *)data + sizeof(int)), (void *)vec->x,
         vec->nUsed * szF);
  return data;
}

Vector *vector_from_blob(void *blob, int size) {
  if (blob == NULL || size == 0) {
    return NULL;
  }
  int dim = *((int *) blob);
  if (dim == 0) {
    return NULL;
  }
  Vector *vec = vector_new(dim);
  float *arr = (float *)((int *)blob + 1);
  while (dim-- > 0) {
    vector_add_item(vec, *arr);
    arr++;
  }
  return vec;
}

Vector *vector_from_json_text(const char *jsonText) {
  Vector *vec;
  cJSON *jsonObj, *item;

  jsonObj = cJSON_Parse(jsonText);
  if (!cJSON_IsArray(jsonObj) || cJSON_GetArraySize(jsonObj) == 0) {
    goto invalid_format;
  }

  vec = vector_new(cJSON_GetArraySize(jsonObj));
  cJSON_ArrayForEach(item, jsonObj) {
    if (!cJSON_IsNumber(item)) goto invalid_format;
    vector_add_item(vec, cJSON_GetNumberValue(item));
  }
  return vec;

invalid_format:
  if (jsonObj != NULL) cJSON_Delete(jsonObj);
  if (vec != NULL) vector_free(vec);
  return NULL;
}

void vector_free(Vector *vec) {
  sqlite3_free(vec->x);
  sqlite3_free(vec);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
    int sqlite3_vector_init(sqlite3 *db, char **pzErrMsg,
                            const sqlite3_api_routines *pApi) {
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg; /* Unused parameter */
  rc = sqlite3_create_function_v2(db, "vector_json_to_blob", 1,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  vector_json_to_blob, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  rc = sqlite3_create_function_v2(db, "vector_blob_to_text", 1,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  vector_blob_to_text, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  rc = sqlite3_create_function_v2(db, "vector_json_to_text", 1,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  vector_json_to_text, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  rc = sqlite3_create_function_v2(db, "vector_version", 0,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  vector_version, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  return rc;
}

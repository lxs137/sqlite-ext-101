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

static void vector_version(sqlite3_context *context, int argc,
                           sqlite3_value **argv) {
  sqlite3_result_text(context, SQLITE_EXT_VERSION, -1, SQLITE_STATIC);
}

static void vector_to_blob(sqlite3_context *context, int argc,
                           sqlite3_value **argv) {
  assert(argc == 1);
  Vector *vec = parse_vector(argv[0]);
  if (vec == NULL) {
    sqlite3_result_text(context, "invalid vector", -1, SQLITE_STATIC);
    return;
  }
  char *vecDetail = sqlite3_mprintf("vector{ dim: %d }", vec->dim);
  sqlite3_result_text(context, vecDetail, strlen(vecDetail), sqlite3_free);
}

Vector *parse_vector(sqlite3_value *value) {
  Vector *vec;
  int dim;
  float *data;

  if (sqlite3_value_type(value) != SQLITE3_TEXT) {
    goto invalid_format;
  }
  const char *valueStr = (const char *)sqlite3_value_text(value);
  cJSON *valueJSON = cJSON_Parse(valueStr);
  if (!cJSON_IsArray(valueJSON) || cJSON_GetArraySize(valueJSON) == 0) {
    goto invalid_format;
  }
  const cJSON *item = NULL;
  vec = vector_new(cJSON_GetArraySize(valueJSON));
  cJSON_ArrayForEach(item, valueJSON) {
    if (!cJSON_IsNumber(item)) goto invalid_format;
    vector_add_item(vec, cJSON_GetNumberValue(item));
  }
  return vec;

invalid_format:
  return NULL;
}

Vector *vector_new(int dim) {
  SQLITE_EXTENSION_INIT3
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

void vector_free(Vector *vec) {
  SQLITE_EXTENSION_INIT3
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
  rc = sqlite3_create_function_v2(db, "vector_to_blob", 1,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  vector_to_blob, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  rc = sqlite3_create_function_v2(db, "vector_version", 0,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  vector_version, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  return rc;
}

#ifndef _SQLITE_EXT_VECTOR_H
#define _SQLITE_EXT_VECTOR_H

#include "sqlite/sqlite3.h"
#include "sqlite/sqlite3ext.h"

#include "cJSON/cJSON.h"

SQLITE_EXTENSION_INIT1

#ifndef SQLITE_EXT_VERSION
#define SQLITE_EXT_VERSION "0.0.0"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// TODO: store x in continuous memory with Vector
struct Vector {
  int dim; /* number of dimensions */
  int nUsed;
  float *x;
};

typedef struct Vector Vector;

// parse vector from string, string format: "[1, 2, 3]"
Vector *parse_vector(sqlite3_value *);

// new nector by dimension
Vector *vector_new(int);

// add item into vector
void vector_add_item(Vector*, float);

// free vector memory
void vector_free(Vector*);

// dump vector into raw data, data size is return by arg1
// raw data size = sizeof(int) + sizeof(float)*vec->nUsed
void *vector_to_blob(Vector*, int*);

// construct vector from raw blob data
Vector *vector_from_blob(void*, int);

// construct vector from json string
Vector *vector_from_json_text(const char*);

int sqlite3_vector_init(sqlite3*, char**, const sqlite3_api_routines*);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _SQLITE_EXT_VECTOR_H */

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

struct Vector {
  int dim; /* number of dimensions */
  int nUsed;
  float *x;
};

typedef struct Vector Vector;

Vector *parse_vector(sqlite3_value *);

Vector *vector_new(int);

void vector_add_item(Vector*, float);

void vector_free(Vector*);

int sqlite3_vector_init(sqlite3*, char**, const sqlite3_api_routines*);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _SQLITE_EXT_VECTOR_H */

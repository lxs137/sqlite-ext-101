/*
** This SQLite extension implements a tick() function used for testing.
**
** Variants:
**
**    tick_version()    print extension version
**    tick(X)           echo X
**    tick_multi(S,L)   echo S in L times, e.g. tick("aa", 3), print [aa, aa, aa]
*/

#include "tick.h"

#include "utils.h"

#include <assert.h>

static void tick_version(sqlite3_context *context, int argc,
                         sqlite3_value **argv) {
  sqlite3_result_text(context, SQLITE_EXT_VERSION, -1, SQLITE_STATIC);
}

static void tick(sqlite3_context *context, int argc, sqlite3_value **argv) {
  assert(argc == 1);
  sqlite3_result_value(context, argv[0]);
}

static void tick_multi(sqlite3_context *context, int argc,
                       sqlite3_value **argv) {
  assert(argc == 2);
  const char *str = (const char *)sqlite3_value_text(argv[0]);
  int count = atoi((const char *)sqlite3_value_text(argv[1]));
  if (count <= 0) return;

  sqlite3_result_value(context, argv[0]);
  sqlite3_result_value(context, argv[1]);

  int resultLen = count*(strlen(str)+1) + 1;
  char *result = sqlite3_malloc(resultLen);
  result[0] = '[';
  while (count-- > 0) {
    strcat(result, str);
    strcat(result, ",");
  }
  result[resultLen-1] = ']';
  result[resultLen] = '\0';
  assert(strlen(result) == resultLen);
  
  sqlite3_result_text(context, result, resultLen, sqlite3_free);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_tick_init(sqlite3 *db, char **pzErrMsg,
                      const sqlite3_api_routines *pApi) {
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg; /* Unused parameter */
  rc = sqlite3_create_function_v2(
      db, "tick", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, tick, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  rc = sqlite3_create_function_v2(
      db, "tick_multi", 2, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, tick_multi, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  rc = sqlite3_create_function_v2(db, "tick_version", 0,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  tick_version, 0, 0, 0);
  if (rc != SQLITE_OK) return rc;
  return rc;
}

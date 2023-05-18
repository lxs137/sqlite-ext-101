/*
** This SQLite extension implements a tick() function used for testing.
**
** Variants:
**
**    tick(X)           echo X
**    tick_version()    print version
*/
#include "tick.h"
SQLITE_EXTENSION_INIT1
#include <assert.h>
#include <string.h>

static void tick(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  assert( argc==1 );
  sqlite3_result_value(context, argv[0]);
}

static void tick_version(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  sqlite3_result_text(context, SQLITE_TICK_VERSION, -1, SQLITE_STATIC);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_tick_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  int rc = SQLITE_OK;
  SQLITE_EXTENSION_INIT2(pApi);
  (void)pzErrMsg;  /* Unused parameter */
  rc = sqlite3_create_function_v2(
      db, "tick", 1, SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0, tick, 0, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  rc = sqlite3_create_function_v2(db, "tick_version", 0,
                                  SQLITE_UTF8 | SQLITE_DETERMINISTIC, 0,
                                  tick_version, 0, 0, 0);
  if( rc != SQLITE_OK ) return rc;
  return rc;
}

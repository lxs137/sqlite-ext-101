#ifndef _SQLITE_TICK_H
#define _SQLITE_TICK_H

#include "sqlite/sqlite3.h"
#include "sqlite/sqlite3ext.h"

#ifndef SQLITE_TICK_VERSION
#define SQLITE_TICK_VERSION "0.0.0"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int sqlite3_tick_init(sqlite3*, char**, const sqlite3_api_routines*);

#ifdef __cplusplus
}
#endif

#endif /* ifndef _SQLITE_TICK_H */
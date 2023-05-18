#ifndef _SQLITE_EXT_UTILS_H
#define _SQLITE_EXT_UTILS_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite/sqlite3.h"
#include "sqlite/sqlite3ext.h"

SQLITE_EXTENSION_INIT3

#ifdef __cplusplus
extern "C" {
#endif

static const char *str_skip_whitespace(const char *str) {
  while (isspace((unsigned char)str[0])) str++;
  return str;
}

// remove trailing whitespace from the end of string
static void str_trim_whitespace(char *str) {
  size_t n = strlen(str);
  while (n > 0 && isspace((unsigned char)str[n])) n--;
  str[n] = 0;
}

// dequote str in place
static void str_dequote(char *str) {
  int j;
  char quote = str[0];
  size_t i, n;

  if (quote != '\'' && quote != '"') return;
  n = strlen(str);
  if (n < 2 || str[n - 1] != str[0]) return;
  for (i = 1, j = 0; i < n - 1; i++) {
    if (str[i] == quote && str[i + 1] == quote) i++;
    str[j] = str[i];
    j++;
  }
  str[j] = 0;
}

/* Check to see if the string is of the form:  "TAG = VALUE" with optional
 * whitespace before and around tokens.  If it is, return a pointer to the
 * first character of VALUE.  If it is not, return 0.
*/
static const char *read_parameter(const char *paramName, const char *argStr) {
  int paramNameLen = (int)strlen(paramName);
  argStr = str_skip_whitespace(argStr);
  if (strncmp(paramName, argStr, paramNameLen) != 0) return 0;
  argStr = str_skip_whitespace(argStr + paramNameLen);
  if (argStr[0] != '=') return 0;
  return str_skip_whitespace(argStr + 1);
}

/* Decode a parameter that requires a dequoted string.
 *
 * Return 1 if the parameter is seen, or 0 if not or has decode error.
*/
static int read_string_parameter(const char *paramName, const char *argStr,
                                 char **paramVal) {
  const char *paramData;
  paramData = read_parameter(paramName, argStr);
  if (paramData == 0) return 0;
  *paramVal = sqlite3_mprintf("%s", paramData);
  if (*paramVal == 0) {
    // out of memory
    return 0;
  }
  str_trim_whitespace(*paramVal);
  str_dequote(*paramVal);
  return 1;
}

#ifdef __cplusplus
}
#endif

#endif /* ifndef _SQLITE_EXT_UTILS_H */

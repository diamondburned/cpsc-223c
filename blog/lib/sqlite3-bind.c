/* ---------------------------------------------------------------------------
** sqlite3-bind: SQLite C API - Parameter binding helper for SQLite.
** ---------------------------------------------------------------------------
** Copyright (c) 2016 by Payton Bissell, payton.bissell@gmail.com
** Copyright (c) 2023 by diamondburned, diamond@libdb.so
** ---------------------------------------------------------------------------
** Permission to use, copy, modify, and/or distribute this software for any
** purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
** OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
**
** Also offered with MIT License.
**
** INTENTION: I, Payton Bissell, the sole author of this source code, dedicate
** any and all copyright interest in this code to the public domain. I make
** this dedication for the benefit of the public at large and to the detriment
** of my heirs and successors. I intend this dedication to be an overt act of
** relinquishment in perpetuity of all present and future rights to this code
** under copyright law.
**
** Copyleft is the antithesis of freedom. Please resist it.
** ---------------------------------------------------------------------------
*/

#include "sqlite3-bind.h"

#include <string.h>

#include "sqlite3.h"

/* ---------------------------------------------------------------------------
** For now the array binding does not support results. It would require a
** modified callback to pass the bound params for the user to know which
** array values are being used to derrive the current results, among other
** complexities. Reconsider Later. Any code that looks ahead to this feature
** will be excluded with this #define.
** ---------------------------------------------------------------------------
*/
#define I_SQLITE_BIND_ARRAY_CANT_HAVE_RESULTS 1

/* ---------------------------------------------------------------------------
** Public function to get error message, intercepts errors that were from
** the sqlite3-bind tool.
** Currently does not associate last error with a database object.
** ---------------------------------------------------------------------------
*/
static int g_last_err_code = 0;
const char *sqlite3_bind_errmsg(sqlite3 *db) {
  switch (g_last_err_code) {
    case SQLITE_ERR_BIND_STACK_NOT_TERMINATED:
      return "sqlite3-bind: no end marker after all params are bound.";
    case SQLITE_ERR_BIND_STACK_MISSING_PARAMS:
      return "sqlite3-bind: end marker before all params are bound.";
    case SQLITE_ERR_BIND_STACK_GUIDE_INVALID:
      return "sqlite3-bind: guide marker was not recognized.";
    case SQLITE_ERR_BIND_RESULT_COLUMNS_COUNT:
      return "the number of result columns does not match setup.";
    case SQLITE_ERR_BIND_ARRAY_CANT_HAVE_RESULTS:
      return "for now the bind_array functions cannot return results.";
  }
  return sqlite3_errmsg(db);
}

/* ***************************************************************************
**      SIMPLE BIND SECTION
** ***************************************************************************
*/

/* ---------------------------------------------------------------------------
** Prototypes for internal functions, not intended for external calls
*/
static int i_bind_exec_va(sqlite3 *db, const void *sql,
                          sqlite3_bind_callback callback, void *arg,
                          va_list params);

/* ---------------------------------------------------------------------------
** Public bind_exec functions stage execution of i_bind_exec_va
** ---------------------------------------------------------------------------
*/
int sqlite3_bind_exec(sqlite3 *db, const char *sql,
                      sqlite3_bind_callback callback, void *arg, ...) {
  va_list params;
  va_start(params, arg);
  int ret = i_bind_exec_va(db, (const void *)sql, callback, arg, params);
  va_end(params);
  return ret;
}

/* ---------------------------------------------------------------------------
 */
int sqlite3_bind_exec_va(sqlite3 *db, const char *sql,
                         sqlite3_bind_callback callback, void *arg,
                         va_list params) {
  return i_bind_exec_va(db, (const void *)sql, callback, arg, params);
}

/* ---------------------------------------------------------------------------
** Check if the sql is completed, or more commands are left to be executed.
** Can check either sql format.
** ---------------------------------------------------------------------------
*/
static int i_check_tail(const char *p1_tail) {
  if (p1_tail == NULL) return 0;
  if (strlen(p1_tail) == 0) return 0;
  return 1;
}

/* ---------------------------------------------------------------------------
** Bind Exec implementation.
** ---------------------------------------------------------------------------
*/
static int i_bind_exec_va(sqlite3 *db, const void *sql,
                          sqlite3_bind_callback callback, void *arg,
                          va_list params) {
  g_last_err_code = SQLITE_OK;
  int i, ret = SQLITE_OK;
  sqlite3_stmt *stmt = NULL;

  // only one is used based on the type of null terminated sql is passed: 1=8bit
  // and 2=16bit.
  const char *p1_tail = (const char *)sql;

  // for each semi-colon separated statement in the sql...
  while ((ret == SQLITE_OK) && (i_check_tail(p1_tail))) {
    int argc = 0, pcnt = 0;

    // prep this statment (of potentially many)...
    if ((ret = sqlite3_prepare_v2(db, p1_tail, -1, &stmt, &p1_tail)) !=
        SQLITE_OK)
      return ret;

    // impossible? prepare should return code above.
    if (stmt == NULL) return SQLITE_ERROR;

    // we have a good statement object, so get param count and column count...
    argc = sqlite3_column_count(stmt);
    pcnt = sqlite3_bind_parameter_count(stmt);

    // bind all the parameters from the stack...
    for (i = 0; ((ret == SQLITE_OK) && (i < pcnt)); i++) {
      // all valid stack params will have a guide marker to denote type, we wont
      // use a stack var without it.
      unsigned int guide = va_arg(params, unsigned int);

      int ni;  // native int
      char *str;
      sqlite3_uint64 i8;
      void *vp;
      double dbl;

      switch (guide) {
        case I_SQLITE_BIND_TYPE_BLOB:
          ni = va_arg(params, int);
          vp = va_arg(params, void *);
          ret = sqlite3_bind_blob(stmt, i + 1, vp, ni, SQLITE_STATIC);
          break;

        case I_SQLITE_BIND_TYPE_DOUBLE:
          dbl = va_arg(params, double);
          ret = sqlite3_bind_double(stmt, i + 1, dbl);
          break;

        case I_SQLITE_BIND_TYPE_INT:
          ni = va_arg(params, int);
          ret = sqlite3_bind_int(stmt, i + 1, ni);
          break;

        case I_SQLITE_BIND_TYPE_INT64:
          i8 = va_arg(params, sqlite3_uint64);
          ret = sqlite3_bind_int64(stmt, i + 1, i8);
          break;

        case I_SQLITE_BIND_TYPE_NULL:
          ret = sqlite3_bind_null(stmt, i + 1);
          break;

        case I_SQLITE_BIND_TYPE_TEXT:
          str = va_arg(params, char *);
          ret = sqlite3_bind_text(stmt, i + 1, str, -1, SQLITE_STATIC);
          break;

        case I_SQLITE_BIND_TYPE_ZBLOB:
          ni = va_arg(params, int);  // size
          ret = sqlite3_bind_zeroblob(stmt, i + 1, ni);
          break;

        // this situation means that the stack had less params than there were
        // param-markers in the sql. or make my own SQLITE error codes.
        case SQLITE_BIND_END:
          ret = g_last_err_code = SQLITE_ERR_BIND_STACK_MISSING_PARAMS;
          break;

        // here means that we don't recognize the guide bytes on the stack.
        default:
          ret = g_last_err_code = SQLITE_ERR_BIND_STACK_GUIDE_INVALID;
      }
    }

    int row = 0;

    // if still good, then execute all the rows one at a time...
    while (ret == SQLITE_OK) {
      // process the statement with the current bindings.
      int r = sqlite3_step(stmt);

      if (r == SQLITE_DONE) {
        // it's done, so we can jump out of the row loop
        break;
      } else if (r != SQLITE_ROW) {
        // anything other than SQLITE_ROW is a some failure.
        ret = r;  // SQLITE_ERROR;
        break;
      }

      // at this point there must be a SQLITE_ROW of data to process...

      // there is no need to mess with the data if the callback is not set, just
      // call _step until DONE
      if (callback == NULL) continue;

      // just a gut check that the result column count matches the statement
      // column count.
      if (argc != sqlite3_data_count(stmt)) {
        ret = g_last_err_code = SQLITE_ERR_BIND_RESULT_COLUMNS_COUNT;
        break;
      }

      // call the callback
      int cr = callback(arg, stmt);
      if (cr != 0)
        break;  // short circuit row loop if the callback returns non-zero
      row++;
    }

    // clean up from the execution
    ret = sqlite3_finalize(stmt);
  }

// if there are extra parameters pushed on the stack there is no harm, but it is
// probably a bug. This can be undefined to tolerate extra stack variables or an
// unterminated stack
#ifndef I_SQLITE_BIND_STACK_NOT_CHECKED
  if (ret == SQLITE_OK)
    if (va_arg(params, unsigned int) != SQLITE_BIND_END)
      ret = g_last_err_code = SQLITE_ERR_BIND_STACK_NOT_TERMINATED;
#endif

  return ret;
}

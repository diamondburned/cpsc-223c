/* ---------------------------------------------------------------------------
** sqlite3-bind: SQLite C API - Parameter binding helper for SQLite.
**
** Code modified to allow more flexibility in the sqlite3_bind_callback. This is
** needed for sqlite3-scanner to work right.
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
** You can disable stack termination checking by defining
**   I_SQLITE_BIND_STACK_NOT_CHECKED prior to sqlite3-bind.h
**   e.g. gcc -DI_SQLITE_BIND_STACK_NOT_CHECKED
**
** ---------------------------------------------------------------------------
*/

#pragma once

#include <sqlite3.h>
#include <stdarg.h> /* Needed for the definition of va_list */

typedef int (*sqlite3_bind_callback)(void *data, sqlite3_stmt *stmt);

/* ---------------------------------------------------------------------------
** Error codes: Since this tool simple wraps SQLite calls, most of the
** return codes are those returned by SQLite. Using negative result/error
** codes until more research can be done.
*/
#define SQLITE_ERR_BIND_STACK_NOT_TERMINATED \
  (-1)  // no end marker after all params are bound
#define SQLITE_ERR_BIND_STACK_MISSING_PARAMS \
  (-2)  // end marker before all params are bound
#define SQLITE_ERR_BIND_STACK_GUIDE_INVALID \
  (-3)  // guide marker was not recognized.
#define SQLITE_ERR_BIND_RESULT_COLUMNS_COUNT \
  (-4)  // the number of result columns does not match setup
#define SQLITE_ERR_BIND_ARRAY_CANT_HAVE_RESULTS \
  (-5)  // for now the bind_array functions cannot return results
        // a query that generates results will create this error

/* ---------------------------------------------------------------------------
** INTERNAL random guide bytes that provide some confidence that the stack
** variables are the correct type
*/
#define I_SQLITE_BIND_TYPE_BLOB ((unsigned int)0x156a82f4)
#define I_SQLITE_BIND_TYPE_DOUBLE ((unsigned int)0x3a4947a1)
#define I_SQLITE_BIND_TYPE_INT ((unsigned int)0xedf19d91)
#define I_SQLITE_BIND_TYPE_INT64 ((unsigned int)0xc3aea174)
#define I_SQLITE_BIND_TYPE_NULL ((unsigned int)0x93640739)
#define I_SQLITE_BIND_TYPE_TEXT ((unsigned int)0xa352651d)
#define I_SQLITE_BIND_TYPE_ZBLOB ((unsigned int)0x7c435b51)

/* ---------------------------------------------------------------------------
** Public function to use in-place of sqlite3_errmsg
*/
const char *sqlite3_bind_errmsg(sqlite3 *db);

/* ---------------------------------------------------------------------------
** User macros for pushing params on the stack for binding, provide guide marker
*and casting
*/
#define SQLITE_BIND_BLOB(b, s) \
  I_SQLITE_BIND_TYPE_BLOB, (int)(s), (const void *)(b)
#define SQLITE_BIND_DOUBLE(d) I_SQLITE_BIND_TYPE_DOUBLE, (double)(d)
#define SQLITE_BIND_INT(i) I_SQLITE_BIND_TYPE_INT, (int)(i)
#define SQLITE_BIND_INT64(i) I_SQLITE_BIND_TYPE_INT64, (sqlite3_int64)(i)
#define SQLITE_BIND_NULL I_SQLITE_BIND_TYPE_NULL
#define SQLITE_BIND_TEXT(t) I_SQLITE_BIND_TYPE_TEXT, (const char *)(t)
#define SQLITE_BIND_ZBLOB(s) I_SQLITE_BIND_TYPE_ZBLOB, (int)(s)
#define SQLITE_BIND_END ((unsigned int)0x87fa3dab)

/* ---------------------------------------------------------------------------
** The sqlite_bind_exec functions follow the sqlite_exec API pattern
** but support variable arguments that will be bound to '?' markers
** in the sql statement.
** ---------------------------------------------------------------------------
*/
int sqlite3_bind_exec(sqlite3 *db, const char *sql,
                      sqlite3_bind_callback callback, void *arg, ...);
int sqlite3_bind_exec_va(sqlite3 *db, const char *sql,
                         sqlite3_bind_callback callback, void *arg,
                         va_list params);

sqlite3_stmt *sqlite3_bind_prepare(sqlite3 *db, const char *sql,
                                   sqlite3_bind_callback callback, void *arg,
                                   ...);
sqlite3_stmt *sqlite3_bind_prepare_va(sqlite3 *db, const char *sql,
                                      sqlite3_bind_callback callback, void *arg,
                                      va_list params);

#pragma once

#include <sqlite3-bind.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  SQLITE_SCAN_END,  // should only appear at the end
  SQLITE_SCAN_TEXT,
  SQLITE_SCAN_BLOB,
  SQLITE_SCAN_INTEGER,
  SQLITE_SCAN_REAL,
  SQLITE_SCAN_NULL,
} sqlite3_scan_item_type;

typedef struct {
  uint8_t *data;
  size_t size;
} sqlite3_scan_blob;

// sqlite3_scan_item is a single item value belonging to a row in the result
// set.
typedef struct sqlite3_scan_item {
  sqlite3_scan_item_type type;
  union {
    char *text;              // SQLITE_SCAN_TEXT
    sqlite3_scan_blob blob;  // SQLITE_SCAN_BLOB
    int64_t integer;         // SQLITE_SCAN_INTEGER
    double real;             // SQLITE_SCAN_REAL
  } value;
} sqlite3_scan_item;

typedef sqlite3_scan_item *sqlite3_scan_row;    // sqlite3_scan_item[]
typedef sqlite3_scan_item **sqlite3_scan_rows;  // sqlite3_scan_item[][]

// sqlite3_scan_rows_callback is the callback to be used for sqlite3_bind_exec.
//
// Example Usage:
//
//    sqlite3_scan_rows rows;
//    sqlite3_bind_exec(db, query, sqlite3_scan_rows_callback, &rows, ...);
//    for (size_t i = 0; rows[i]; i++) {
//      for (size_t j = 0; rows[i][j].type != SQLITE_SCAN_END; i++) {
//        switch (rows[i][j].type) {
//          case SQLITE_SCAN_TEXT:
//            printf("text: %s\n", rows[i][j].value.text);
//            break;
//          }
//        }
//      }
//    }
//    sqlite3_scan_free_rows(items);
//
// transfer: none
int sqlite3_scan_rows_callback(void *data, sqlite3_stmt *stmt);

// sqlite3_scan_rows_count returns the count of the given rows array.
int sqlite3_scan_rows_count(sqlite3_scan_rows rows);

// sqlite3_scan_row_callback is the callback to be used for sqlite3_bind_exec.
// It works similarly to sqlite3_scan_callback, but it only returns one row.
// As such, the user must pass a value of type sqlite3_scan_row*.
int sqlite3_scan_row_callback(void *data, sqlite3_stmt *stmt);

// sqlite3_scan_row_count returns the count of the given row, which is the
// number of columns scanned.
int sqlite3_scan_row_count(sqlite3_scan_row row);

// Funny functions:

// sqlite3_scan_free_rows frees a sqlite3_scan_rows instance.
//
// transfer: full
void sqlite3_scan_free_rows(sqlite3_scan_rows *rows);

// sqlite3_scan_free_row frees a sqlite3_scan_row instance.
//
// transfer: full
void sqlite3_scan_free_row(sqlite3_scan_row *row);

// sqlite3_scan_free frees the given item.
//
// transfer: full
void sqlite3_scan_free(sqlite3_scan_item **item);

void sqlite3_must_begin(sqlite3 *db);
void sqlite3_must_commit(sqlite3 *db);

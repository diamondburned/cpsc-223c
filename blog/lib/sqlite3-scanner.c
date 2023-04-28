#include "sqlite3-scanner.h"

#include <free2.h>
#include <panic.h>
#include <stdlib.h>
#include <strdup.h>
#include <string.h>

void sqlite3_scan_single(sqlite3_stmt *stmt, int i, sqlite3_scan_item *item) {
  switch (sqlite3_column_type(stmt, i)) {
    case SQLITE_INTEGER: {
      item->type = SQLITE_SCAN_INTEGER;
      item->value.integer = sqlite3_column_int64(stmt, i);
      break;
    }
    case SQLITE_FLOAT: {
      item->type = SQLITE_SCAN_REAL;
      item->value.real = sqlite3_column_double(stmt, i);
      break;
    }
    case SQLITE_TEXT: {
      item->type = SQLITE_SCAN_TEXT;
      // We all use UTF-8 now, right? :)
      item->value.text = strdup((const char *)sqlite3_column_text(stmt, i));
      break;
    }
    case SQLITE_BLOB: {
      sqlite3_column_blob(stmt, i);  // force cast to blob first, manual says so
      item->type = SQLITE_SCAN_BLOB;
      item->value.blob.size = sqlite3_column_bytes(stmt, i);
      item->value.blob.data =
          memdup(sqlite3_column_blob(stmt, i), item->value.blob.size);
      break;
    }
    case SQLITE_NULL: {
      item->type = SQLITE_SCAN_NULL;
      break;
    }
    default: {
      panic("Unknown column type: %d", sqlite3_column_type(stmt, i));
    }
  }
}

int sqlite3_scan_row_callback(void *data, sqlite3_stmt *stmt) {
  sqlite3_scan_item **row = data;
  if (*row != NULL) {
    // be careful!
    panic("Multiple rows returned");
  }

  int n_col = sqlite3_data_count(stmt);

  sqlite3_scan_item *items = calloc(sizeof(sqlite3_scan_item), (n_col + 1));
  items[n_col].type = SQLITE_SCAN_END;

  for (int i = 0; i < n_col; i++) {
    sqlite3_scan_item *item = &items[i];
    sqlite3_scan_single(stmt, i, item);
  }

  *row = items;
  return 0;
}

int sqlite3_scan_rows_callback(void *data, sqlite3_stmt *stmt) {
  // scan_rows_callback is called every time sqlite3_step() returns.
  // This means that there's no way to know how many rows there are
  // until we've reached the end of the result set.
  //
  // We'll have to keep calling realloc() until we've reached the end.
  sqlite3_scan_rows *rows = data;

  // There's a small hack that we'll use here. We'll keep a static variable
  // around that indicates the capacity of the array. If *rows is NULL, then
  // we'll reset the static capacity to 0, since we're starting over.
  static int capacity;
  static int n_col;
  static int n_row;
  if (*rows == NULL) {
    capacity = 0;
    n_col = sqlite3_data_count(stmt);
    n_row = 0;
  }

  n_row++;
  if (n_row + 1 >= capacity) {  // add 1 for NULL
    if (capacity < 4) {
      // This works around the case where we're only scanning a single row.
      // Without it, (capacity*2) would be 0. 4 is also a decent starting point.
      capacity = 4;
    } else {
      capacity *= 2;
    }
    *rows = realloc(*rows, sizeof(sqlite3_scan_row) * capacity);
  }

  sqlite3_scan_row row = calloc(sizeof(sqlite3_scan_item), (n_col + 1));
  row[n_col].type = SQLITE_SCAN_END;

  for (int i = 0; i < n_col; i++) {
    sqlite3_scan_single(stmt, i, &row[i]);
  }

  (*rows)[n_row - 1] = row;
  (*rows)[n_row] = NULL;

  return 0;
}

// transfer: container
void sqlite3_scan_free_inner(sqlite3_scan_item *item) {
  switch (item->type) {
    case SQLITE_SCAN_TEXT: {
      free2_owned((void **)&item->value.text);
      break;
    }
    case SQLITE_SCAN_BLOB: {
      free2_owned((void **)&item->value.blob.data);
      break;
    }
    default: {
      break;
    }
  }
}

// transfer: full
void sqlite3_scan_free(sqlite3_scan_item **item) {
  if (*item != NULL) {
    sqlite3_scan_free_inner(*item);
    free2_owned((void **)item);
  }
}

// transfer: full
void sqlite3_scan_free_row(sqlite3_scan_row *row) {
  if (*row != NULL) {
    sqlite3_scan_item *items = *row;
    for (int i = 0; items[i].type != SQLITE_SCAN_END; i++) {
      sqlite3_scan_free_inner(&items[i]);
    }
    free2_owned((void **)row);
  }
}

// transfer: full
void sqlite3_scan_free_rows(sqlite3_scan_rows *rows) {
  if (*rows != NULL) {
    sqlite3_scan_row *row = *rows;
    for (int i = 0; row[i] != NULL; i++) {
      sqlite3_scan_free_row(&row[i]);
    }
    free2_owned((void **)rows);
  }
}

int sqlite3_scan_rows_count(sqlite3_scan_rows rows) {
  if (rows == NULL) {
    return 0;
  }

  int count = 0;
  for (int i = 0; rows[i] != NULL; i++) {
    count++;
  }
  return count;
}

int sqlite3_scan_row_count(sqlite3_scan_row row) {
  int count = 0;
  for (int i = 0; row[i].type != SQLITE_SCAN_END; i++) {
    count++;
  }
  return count;
}

void sqlite3_must_begin(sqlite3 *db) {
  int rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    panic("Failed to begin transaction: %s", sqlite3_errmsg(db));
  }
};

void sqlite3_must_commit(sqlite3 *db) {
  int rc = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, NULL);
  if (rc != SQLITE_OK) {
    panic("Failed to commit transaction: %s", sqlite3_errmsg(db));
  }
};

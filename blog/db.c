#include "db.h"

#include <panic.h>
#include <sqlite3.h>

#include "blog/blog.h"

sqlite3* db = NULL;

void db_init(const char* path) {
  if (db != NULL) {
    panic("db_init called twice");
  }

  int rc = sqlite3_open(path, &db);
  if (rc != SQLITE_OK) {
    panic("cannot open database: %s", sqlite3_errmsg(db));
  }
  blog_init_schema(db);
}

void db_init_test() {
  if (db != NULL) {
    panic("db_init called twice");
  }

  int rc = sqlite3_open(":memory:", &db);
  if (rc != SQLITE_OK) {
    panic("cannot open database on memory: %s", sqlite3_errmsg(db));
  }
  blog_init_schema(db);
}

void db_close() {
  while (true) {
    int status = sqlite3_close(db);
    if (status == SQLITE_OK) {
      db = NULL;
      return;
    }
    // Database might still be in use by other threads.
    // We can't really "wait" for them to finish, since we're not in a threaded
    // environment. Just explode instead.
    panic("cannot close database: %s", sqlite3_errmsg(db));
  }
}

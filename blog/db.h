#pragma once

#include <panic.h>
#include <sqlite3.h>

#include "blog/blog.h"

extern sqlite3* db;  // defined in db.c

// db_init initializes the db singleton.
void db_init(const char* path);

// db_init_test initializes the db singleton for testing. It creates a temporary
// database in memory.
void db_init_test();

// db_close closes the db singleton.
void db_close();

#include "blog.h"

#include <free2.h>
#include <panic.h>
#include <regex.h>
#include <sqlite3-bind.h>
#include <sqlite3-scanner.h>
#include <sqlite3.h>
#include <strdup.h>
#include <string.h>

#include "password.h"
#include "schema.sql.h"

void blog_init_schema(sqlite3* db) {
  static char* schema_str;
  if (schema_str == NULL) {
    schema_str = malloc(blog_schema_sql_len + 1);
    memcpy(schema_str, blog_schema_sql, blog_schema_sql_len);
    schema_str[blog_schema_sql_len] = '\0';
  }

  char* err;
  int rc = sqlite3_exec(db, schema_str, NULL, NULL, &err);
  if (rc != SQLITE_OK) {
    panic("Failed to initialize schema: %s", err);
  }
}

const char* BLOG_VALID_USERNAME_REGEX = "^[a-zA-Z0-9\\-_]{3,16}$";

bool blog_validate_username(const char* username) {
  static regex_t regex;
  static bool initialized = false;
  if (!initialized) {
    int rc = regcomp(&regex, BLOG_VALID_USERNAME_REGEX, REG_EXTENDED);
    if (rc != 0) {
      panic("Failed to compile username regex");
    }
    initialized = true;
  }

  return regexec(&regex, username, 0, NULL, 0) == 0;
}

bool blog_user_create(sqlite3* db, const char* user, const char* pass) {
  if (!blog_validate_username(user)) {
    return false;
  }

  char* hash = blog_password_hash(pass);
  if (hash == NULL) {
    panic("Failed to hash password");
  }

  int rc = sqlite3_bind_exec(
      db, "INSERT INTO users (username, passhash) VALUES (?, ?)", NULL, NULL,
      SQLITE_BIND_TEXT(user), SQLITE_BIND_TEXT(hash), SQLITE_BIND_END);
  if (rc == SQLITE_CONSTRAINT) {
    return false;  // UNIQUE constraint failed, so user already exists
  }
  if (rc != SQLITE_OK) {
    panic("Failed to insert user: %s", sqlite3_bind_errmsg(db));
  }

  free(hash);
  return true;
}

blog_user* blog_user_get(sqlite3* db, const char* username) {
  sqlite3_scan_row row = NULL;
  int rc =
      sqlite3_bind_exec(db, "SELECT avatar_url FROM users WHERE username = ?",
                        sqlite3_scan_row_callback, &row,  //
                        SQLITE_BIND_TEXT(username), SQLITE_BIND_END);
  if (rc != SQLITE_OK) {
    panic("Failed to query for user: %s", sqlite3_bind_errmsg(db));
  }
  if (row == NULL) {
    return NULL;
  }

  blog_user* user = malloc(sizeof(blog_user));
  user->username = strdup(username);
  user->avatar_url = strdup(row[0].value.text);

  sqlite3_scan_free_row(&row);
  return user;
}

bool blog_user_verify(sqlite3* db, const char* username, const char* pass) {
  sqlite3_scan_row row = NULL;
  int rc =
      sqlite3_bind_exec(db, "SELECT passhash FROM users WHERE username = ?",
                        sqlite3_scan_row_callback, &row,  //
                        SQLITE_BIND_TEXT(username), SQLITE_BIND_END);
  if (rc != SQLITE_OK || row == NULL) {
    sqlite3_scan_free_row(&row);
    return false;
  }

  const char* hash = row[0].value.text;
  bool valid = blog_password_verify(pass, hash);

  sqlite3_scan_free_row(&row);
  return valid;
}

void blog_user_free(blog_user** user) {
  if (*user != NULL) {
    free2_owned((void**)&(*user)->username);
    free2_owned((void**)&(*user)->avatar_url);
    free2_owned((void**)user);
  }
}

blog_article* blog_article_get(sqlite3* db, blog_article_id id) {
  sqlite3_scan_row row = NULL;
  int rc = sqlite3_bind_exec(
      db, "SELECT title, body, author, created_at FROM articles WHERE id = ?",
      sqlite3_scan_row_callback, &row,  //
      SQLITE_BIND_INT64(id), SQLITE_BIND_END);
  if (rc != SQLITE_OK) {
    panic("Failed to query for article: %s", sqlite3_errmsg(db));
  }
  if (row == NULL) {
    return NULL;
  }

  blog_article* article = malloc(sizeof(blog_article));
  article->id = id;
  article->title = strdup(row[0].value.text);  // my life is a parody lolmao
  article->body = strdup(row[1].value.text);
  article->author = strdup(row[2].value.text);
  article->created_at = row[3].value.integer;

  sqlite3_scan_free_row(&row);
  return article;
}

const int PAGE_COUNT = 10;

blog_article* blog_article_list(sqlite3* db, int page) {
  sqlite3_scan_rows rows = NULL;
  int rc = sqlite3_bind_exec(
      db,
      "SELECT id, title, body, author, created_at FROM articles "
      "ORDER BY created_at DESC LIMIT ? OFFSET ?",
      sqlite3_scan_rows_callback, &rows,  //
      SQLITE_BIND_INT64(PAGE_COUNT), SQLITE_BIND_INT64(page * PAGE_COUNT),
      SQLITE_BIND_END);
  if (rc != SQLITE_OK) {
    panic("Failed to count articles: %s", sqlite3_errmsg(db));
  }

  int n_articles = sqlite3_scan_rows_count(rows);
  if (n_articles == 0) {
    sqlite3_scan_free_rows(&rows);
    return NULL;
  }

  blog_article* articles = malloc(sizeof(blog_article) * (n_articles + 1));
  articles[n_articles].id = 0;

  for (int i = 0; i < n_articles; i++) {
    sqlite3_scan_row row = rows[i];
    blog_article* article = &articles[i];
    article->id = row[0].value.integer;
    article->title = strdup(row[1].value.text);
    article->body = strdup(row[2].value.text);
    article->author = strdup(row[3].value.text);
    article->created_at = row[4].value.integer;
  }

  sqlite3_scan_free_rows(&rows);
  return articles;
}

void blog_article_create(sqlite3* db, blog_article* article) {
  sqlite3_scan_row row = NULL;
  int rc = sqlite3_bind_exec(
      db,
      "INSERT INTO articles (title, body, author) "
      "VALUES (?, ?, ?) RETURNING id, created_at",
      sqlite3_scan_row_callback, &row,  //
      SQLITE_BIND_TEXT(article->title), SQLITE_BIND_TEXT(article->body),
      SQLITE_BIND_TEXT(article->author), SQLITE_BIND_END);
  if (rc != SQLITE_OK) {
    if (rc == SQLITE_CONSTRAINT) {
      // Don't let a non-existing user create an article.
      // We assume the caller has already verified the user exists.
      return;
    }
    panic("Failed to insert article: %s", sqlite3_errmsg(db));
  }

  article->id = row[0].value.integer;
  article->created_at = row[1].value.integer;

  sqlite3_scan_free_row(&row);
}

void blog_article_free_inner(blog_article* article) {
  free2_owned((void**)&article->title);
  free2_owned((void**)&article->body);
  free2_owned((void**)&article->author);
}

void blog_article_free(blog_article** article) {
  if (*article != NULL) {
    blog_article_free_inner(*article);
    free2_owned((void**)article);
  }
}

void blog_articles_free(blog_article** articles) {
  if (*articles != NULL) {
    for (int i = 0; (*articles)[i].id != 0; i++) {
      blog_article_free_inner(&(*articles)[i]);
    }
    free2_owned((void**)articles);
  }
}

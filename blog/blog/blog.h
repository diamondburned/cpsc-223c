#pragma once

#include <sqlite3.h>
#include <stdbool.h>
#include <stdint.h>

// blog_init_schema initializes the database schema into the given sqlite3
// database. It panics if the schema cannot be initialized.
void blog_init_schema(sqlite3* db);

typedef uint64_t blog_article_id;

// blog_validate_username returns true if the username is valid.
bool blog_validate_username(const char* username);

typedef struct {
  char* username;
  char* avatar_url;
} blog_user;

// blog_user_create creates a new user in the database. If the username already
// exists, it returns false.
bool blog_user_create(sqlite3* db, const char* user, const char* pass);

// blog_user_get returns the user with the given ID. If the user does not exist,
// NULL is returned.
//
// transfer: full
blog_user* blog_user_get(sqlite3* db, const char* username);

// blog_user_verify returns true if the given password is valid for the given
// username. If the user does not exist, it returns false.
bool blog_user_verify(sqlite3* db, const char* user, const char* pass);

// blog_user_free frees the given user.
//
// transfer: full
void blog_user_free(blog_user** user);

typedef struct {
  blog_article_id id;
  char* author;
  char* title;
  char* body;
  int64_t created_at;
} blog_article;

// blog_article_get returns the article with the given ID. If an article is not
// found, it returns NULL with no error.
//
// transfer: full
blog_article* blog_article_get(sqlite3* db, blog_article_id id);

// blog_article_list returns a list of articles. An ID of 0 means the end of
// the list.
//
// transfer: full
blog_article* blog_article_list(sqlite3* db, int page);

// blog_article_create creates a new article. The new ID is set in the article
// parameter along with the created_at timestamp. If the article cannot be
// created, the function panics.
//
// transfer: none
void blog_article_create(sqlite3* db, blog_article* article);

// blog_article_free frees the memory allocated by blog_article_get.
//
// transfer: full
void blog_article_free(blog_article** article);

// blog_articles_free frees the memory allocated by blog_article_list.
//
// transfer: full
void blog_articles_free(blog_article** articles);

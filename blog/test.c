#include <free2.h>
#include <minctest.h>
#include <stdbool.h>

#include "blog/blog.h"
#include "db.h"
#include "handlers_private.h"

void test_free2() {
  void* ptr = malloc(10);
  free2_owned(&ptr);
  lok(ptr == NULL);
}

void test_blog_user() {
  blog_user* user1 = blog_user_get(db, "user1");
  lok(user1 == NULL);

  blog_user* user2 = blog_user_get(db, "user2");
  lok(user2 == NULL);

  lok(blog_user_create(db, "user1", "password1"));
  lok(blog_user_create(db, "user2", "password2"));

  user1 = blog_user_get(db, "user1");
  lok(user1 != NULL);
  lsequal("user1", user1->username);
  blog_user_free(&user1);

  user2 = blog_user_get(db, "user2");
  lok(user2 != NULL);
  lsequal("user2", user2->username);
  blog_user_free(&user2);

  lok(blog_user_verify(db, "user1", "password1"));
  lok(blog_user_verify(db, "user2", "password2"));
  lok(!blog_user_verify(db, "user1", "password2"));
  lok(!blog_user_verify(db, "user2", "password1"));
}

void test_blog_article() {
  lok(blog_user_create(db, "user1", "password1"));

  blog_article* articles = blog_article_list(db, 0);
  lok(articles == NULL);  // no articles yet

  blog_article article1 = {
      .title = "title1",
      .body = "body1",
      .author = "user1",
  };
  blog_article_create(db, &article1);
  lequal((int)article1.id, 1);

  blog_article* article1_copy = blog_article_get(db, article1.id);
  lok(article1_copy != NULL);
  lequal((int)article1.id, (int)article1_copy->id);
  lsequal(article1.title, article1_copy->title);
  lsequal(article1.body, article1_copy->body);
  lsequal(article1.author, article1_copy->author);
  blog_article_free(&article1_copy);

  articles = blog_article_list(db, 0);
  lok(articles != NULL);
  lequal((int)articles[0].id, (int)article1.id);
  lsequal(articles[0].title, article1.title);
  lsequal(articles[0].body, article1.body);
  lsequal(articles[0].author, article1.author);
  blog_articles_free(&articles);
}

void test_authorize() {
  lok(blog_user_create(db, "username", "password"));

  const char* username =
      authorize_basic_value("Basic dXNlcm5hbWU6cGFzc3dvcmQ=");
  lsequal("username", username);
  free((void*)username);
}

struct test {
  const char* name;
  void (*func)();
};

int main() {
  struct test tests[] = {
      {.name = "blog/blog_user", .func = test_blog_user},
      {.name = "blog/blog_article", .func = test_blog_article},
      {.name = "handlers_private/authorize", .func = test_authorize},
      {},
  };

  for (int i = 0; tests[i].name; i++) {
    db_init_test();
    lrun(tests[i].name, tests[i].func);
    db_close();
  }

  lresults();
  return lfails != 0;
}

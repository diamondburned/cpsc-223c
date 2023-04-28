#include "handlers.h"

#include <json-c/json.h>
#include <json-c/json_object.h>
#include <onion/block.h>
#include <onion/codecs.h>
#include <onion/onion.h>
#include <onion/response.h>
#include <onion/shortcuts.h>
#include <stdio.h>
#include <stdlib.h>
#include <strdup.h>
#include <string.h>
#include <sys/stat.h>

#include "blog/blog.h"
#include "db.h"
#include "handlers_private.h"
#include "onion/exportlocal.h"

const uint32_t ADD_FLAGS =
    JSON_C_OBJECT_ADD_CONSTANT_KEY | JSON_C_OBJECT_KEY_IS_CONSTANT;

void article_object(const blog_article* article, json_object* obj,
                    bool include_body) {
  json_object_object_add_ex(obj, "id", json_object_new_int(article->id),
                            ADD_FLAGS);
  json_object_object_add_ex(obj, "title",
                            json_object_new_string(article->title), ADD_FLAGS);
  json_object_object_add_ex(obj, "author",
                            json_object_new_string(article->author), ADD_FLAGS);
  json_object_object_add_ex(
      obj, "created_at", json_object_new_int(article->created_at), ADD_FLAGS);
  if (include_body) {
    json_object_object_add_ex(obj, "body",
                              json_object_new_string(article->body), ADD_FLAGS);
  }
}

void user_object(const blog_user* user, json_object* obj) {
  json_object_object_add_ex(obj, "username",
                            json_object_new_string(user->username), ADD_FLAGS);
  json_object_object_add_ex(obj, "avatar_url",
                            user->avatar_url != NULL
                                ? json_object_new_string(user->avatar_url)
                                : NULL,
                            ADD_FLAGS);
}

int route_articles(void* _, onion_request* req, onion_response* res) {
  switch (onion_request_get_flags(req) & OR_METHODS) {
    case OR_GET: {
      const char* page_str = onion_request_get_query(req, "page");
      const int page = page_str ? atoi(page_str) : 0;

      blog_article* articles = blog_article_list(db, page);
      if (articles == NULL) {
        onion_response_write0(res, "[]");
        return OCS_PROCESSED;
      }

      struct json_object* arr = json_object_new_array();
      for (int i = 0; articles[i].id != 0; i++) {
        struct json_object* obj = json_object_new_object();
        article_object(&articles[i], obj, false);
        json_object_array_add(arr, obj);
      }

      onion_response_write0(res, json_object_to_json_string(arr));

      json_object_put(arr);
      blog_articles_free(&articles);
      return OCS_PROCESSED;
    }
    case OR_POST: {
      const char* username = authorize(req);
      if (username == NULL) {
        write_error(res, 401, "Unauthorized");
        return OCS_PROCESSED;
      }

      struct json_object* obj = parse_json_body(req, res);
      if (obj == NULL) {
        return OCS_PROCESSED;
      }

      const char* title =
          json_object_get_string(json_object_object_get(obj, "title"));
      const char* body =
          json_object_get_string(json_object_object_get(obj, "body"));

      if (title == NULL || body == NULL) {
        write_error(res, 400, "Missing title or body");
        return OCS_PROCESSED;
      }

      // Do NOT free this.
      // We don't have ownership of the strings in the object.
      blog_article article = {
          .title = (char*)title,
          .body = (char*)body,
          .author = (char*)username,
      };

      blog_article_create(db, &article);

      obj = json_object_new_object();
      article_object(&article, obj, true);

      onion_response_write0(res, json_object_to_json_string(obj));

      json_object_put(obj);
      free((void*)username);
      return OCS_PROCESSED;
    }
    default: {
      write_error(res, 405, "Method Not Allowed");
      return OCS_PROCESSED;
    }
  }
}

int route_article(void* _, onion_request* req, onion_response* res) {
  const char* query = onion_request_get_query(req, "1");  // get URL parameter
  blog_article_id id = atol(query);

  switch (onion_request_get_flags(req) & OR_METHODS) {
    case OR_GET: {
      blog_article* article = blog_article_get(db, id);
      if (article == NULL) {
        write_error(res, 404, "Article not found");
        return OCS_PROCESSED;
      }

      struct json_object* obj = json_object_new_object();
      article_object(article, obj, true);

      onion_response_write0(res, json_object_to_json_string(obj));

      json_object_put(obj);
      blog_article_free(&article);
      return OCS_PROCESSED;
    }
    default: {
      write_error(res, 405, "Method Not Allowed");
      return OCS_PROCESSED;
    }
  }
}

int route_users(void* _, onion_request* req, onion_response* res) {
  switch (onion_request_get_flags(req) & OR_METHODS) {
    // TODO: OR_GET
    case OR_POST: {
      struct json_object* obj = parse_json_body(req, res);
      if (obj == NULL) {
        return OCS_PROCESSED;
      }

      const char* username =
          json_object_get_string(json_object_object_get(obj, "username"));
      const char* password =
          json_object_get_string(json_object_object_get(obj, "password"));

      if (username == NULL || password == NULL) {
        write_error(res, 400, "Missing username or password");
        return OCS_PROCESSED;
      }

      if (!blog_validate_username(username)) {
        write_error(res, 400, "Invalid username");
        return OCS_PROCESSED;
      }

      if (!blog_user_create(db, username, password)) {
        write_error(res, 400, "User already exists");
        return OCS_PROCESSED;
      };

      onion_response_set_code(res, 204);  // No Content

      json_object_put(obj);
      return OCS_PROCESSED;
    }
    default: {
      write_error(res, 405, "Method Not Allowed");
      return OCS_PROCESSED;
    }
  }
}

int route_user(void* _, onion_request* req, onion_response* res) {
  const char* username = onion_request_get_query(req, "1");
  if (!blog_validate_username(username)) {
    write_error(res, 400, "Invalid username");
    return OCS_PROCESSED;
  }

  switch (onion_request_get_flags(req) & OR_METHODS) {
    case OR_GET: {
      blog_user* user = blog_user_get(db, username);
      if (user == NULL) {
        write_error(res, 404, "User not found");
        return OCS_PROCESSED;
      }

      struct json_object* obj = json_object_new_object();
      user_object(user, obj);

      onion_response_write0(res, json_object_to_json_string(obj));

      json_object_put(obj);
      blog_user_free(&user);
      return OCS_PROCESSED;
    }
    default: {
      write_error(res, 405, "Method Not Allowed");
      return OCS_PROCESSED;
    }
  }
}

int serve_static_file(void* data, onion_request* req, onion_response* res) {
  const char* filename = (const char*)data;

  onion_response_set_header(res, "Cache-Control", "public, max-age=86400");
  return onion_shortcut_response_file(filename, req, res);
}

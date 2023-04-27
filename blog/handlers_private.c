#include "handlers_private.h"

#include <json-c/json.h>
#include <onion/block.h>
#include <onion/codecs.h>
#include <onion/onion.h>
#include <stdio.h>
#include <stdlib.h>
#include <strdup.h>
#include <string.h>

#include "blog/blog.h"
#include "db.h"

void write_error(onion_response* res, int status, const char* error) {
  onion_response_set_code(res, status);

  struct json_object* obj = json_object_new_object();
  json_object_object_add(obj, "error", json_object_new_string(error));
  onion_response_write0(res, json_object_to_json_string(obj));
  json_object_put(obj);
}

struct json_object* parse_json_body(onion_request* req, onion_response* res) {
  const onion_block* data = onion_request_get_data(req);
  if (data == NULL) {
    write_error(res, 400, "No data (missing Content-Length?)");
    return NULL;
  }

  const char* body = onion_block_data(data);
  const int body_size = onion_block_size(data);

  struct json_tokener* tok = json_tokener_new();
  struct json_object* obj = json_tokener_parse_ex(tok, body, body_size);
  if (obj == NULL) {
    if (tok->err != json_tokener_continue) {
      const char* error = json_tokener_error_desc(tok->err);
      write_error(res, 400, error);
    } else {
      write_error(res, 400, "Invalid JSON");
    }

    json_tokener_free(tok);
    return NULL;
  }

  json_tokener_free(tok);
  return obj;
}

char* authorize_basic_value(const char* auth_value) {
  const char* basic_prefix = "Basic ";
  if (strlen(auth_value) < strlen(basic_prefix) ||
      strncmp(auth_value, basic_prefix, strlen(basic_prefix)) != 0) {
    return NULL;
  }

  const char* encoded = &auth_value[strlen(basic_prefix)];

  int decoded_size;
  char* decoded;
  decoded = onion_base64_decode(encoded, &decoded_size);
  decoded = realloc(decoded, decoded_size + 1);
  decoded[decoded_size] = '\0';

  char* colon = strchr(decoded, ':');
  if (!colon) {
    free(decoded);
    return NULL;
  }

  // Hack to divide the string into two parts.
  *colon = '\0';
  const char* given_username = decoded;
  const char* given_password = &colon[1];

  if (!blog_user_verify(db, given_username, given_password)) {
    free(decoded);
    return NULL;
  }

  char* username = strdup(given_username);
  free(decoded);

  return username;
}

// authorize authorizes the request by checking the Authorization header. It
// sets the given user pointer to the user if the request is authorized.
// Otherwise, false is returned.
//
// Currently, it only supports the "Basic" scheme. More may be added in the
// future. Just kidding, I don't like C that much.
char* authorize(onion_request* req) {
  const char* auth_value = onion_request_get_header(req, "Authorization");
  if (!auth_value) {
    return NULL;
  }
  return authorize_basic_value(auth_value);
}

#include "handlers.h"

#include <json-c/json.h>
#include <onion.h>
#include <stdio.h>

void write_error(onion_response* res, int status, char* error) {
  onion_response_set_code(res, status);

  struct json_object* obj = json_object_new_object();
  json_object_object_add(obj, "error", json_object_new_string(error));
  onion_response_write0(res, json_object_to_json_string(obj));
  json_object_put(obj);
}

int route_get_articles(void* _, onion_request* req, onion_response* res) {
  return OCS_NOT_IMPLEMENTED;
}

int route_get_article(void* _, onion_request* req, onion_response* res) {
  const char* path = onion_request_get_path(req);
  const char* query = onion_request_get_query(req, "1");
  printf("path: %s\n", path);
  printf("query: %s\n", query);

  onion_response_write0(res, "Hello, world!");
  return OCS_PROCESSED;
}

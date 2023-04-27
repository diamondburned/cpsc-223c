#include <json-c/json.h>
#include <onion/onion.h>
#include <stdbool.h>

#include "blog/blog.h"
#include "handlers.h"

struct json_object* parse_json_body(onion_request* req, onion_response* res);
void write_error(onion_response* res, int status, const char* error);
char* authorize_basic_value(const char* auth_value);
char* authorize(onion_request* req);

#pragma once

#include <onion.h>

int route_get_articles(void* _, onion_request* req, onion_response* res);
int route_get_article(void* _, onion_request* req, onion_response* res);

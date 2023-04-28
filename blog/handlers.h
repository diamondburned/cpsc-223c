#pragma once

#include <onion/onion.h>

// route_articles: /articles
int route_articles(void* _, onion_request* req, onion_response* res);

// route_article: /articles/:id
int route_article(void* _, onion_request* req, onion_response* res);

// route_users: /users
int route_users(void* _, onion_request* req, onion_response* res);

// route_user: /users/:username
int route_user(void* _, onion_request* req, onion_response* res);

// route_index: /
int route_index(void* _, onion_request* req, onion_response* res);

// serve_static_file is like onion_shortcut_response_file, except it
// also sets the Cache-Control header.
int serve_static_file(void* data, onion_request* req, onion_response* res);

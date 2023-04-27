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

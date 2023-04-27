#pragma once

#include <stdbool.h>

// blog_password_hash hashes the given password and returns it as a string. It
// panics if the password cannot be hashed.
//
// transfer: full
char* blog_password_hash(const char* pass);

// blog_password_validate returns true if the given password is valid for the
// given hash.
bool blog_password_verify(const char* pass, const char* hash);

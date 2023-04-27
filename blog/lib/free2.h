#pragma once

// free2_owned frees an owned pointer. It sets the underlying pointer to NULL
// after freeing it to prevent double frees and use-after-frees. Only use this
// function (or free at all) if you own the pointer.
void free2_owned(void** ptr);

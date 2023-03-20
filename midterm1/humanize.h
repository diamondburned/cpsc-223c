#include <stddef.h>
#include <stdint.h>

// sprintn prints the given number into the given buffer as a humanized English
// string. It returns the number of bytes written to the buffer, not including
// the terminating null byte.
//
// If the number is one quadrillion or larger, it is printed as a decimal
// number.
size_t sprintn(char* buf, int64_t num);

// upper_sentence changes the first letter of the given string to uppercase.
// It only works on ASCII strings.
void upper_sentence(char* str);

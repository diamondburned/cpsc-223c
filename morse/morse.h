typedef enum {
  MORSE_NO_ERROR,
  MORSE_INVALID_CHAR,  // unknown characte
  MORSE_INVALID_CODE,  // unknown code
} morse_error_t;

char* morse_error_string(morse_error_t error);
char* string_to_morse(char* input_buf, morse_error_t* error_code);
char* morse_to_string(char* input_buf, morse_error_t* error_code);

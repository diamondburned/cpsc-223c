#include "morse.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "morse.txt.h"

void panic(const char* msg) {
  fprintf(stderr, "%s\n", msg);
  abort();
}

typedef struct {
  char* str;
  int len;
} morse_string;

bool morse_initialized = false;
char* morse_table[128];          // char -> morse code
morse_string string_table[128];  // morse code -> char

const int MORSE_TABLE_MAX = sizeof(morse_table) / sizeof(morse_table[0]);

// morse_lookup does a lookup on the morse code table. If the character is
// unknown, then NULL is returned.
char* morse_lookup(char c) {
  c = toupper(c);
  if (c < 0 || (int)c >= MORSE_TABLE_MAX) {
    return NULL;
  }
  return morse_table[(int)c];
}

void morse_init() {
  if (morse_initialized) {
    return;
  }

  morse_initialized = true;

  char* txt = (char*)&morse_txt[0];
  while (true) {
    char* line = strtok(txt, "\n");
    if (line == NULL) {
      break;
    }

    // strtok is really funny. It stores the data being parsed globally, so the
    // next call to strtok wants a NULL as the first argument.
    txt = NULL;

    // Assume first character is the letter and the second one is always a tab
    // character. The rest is the morse code.
    char letter = line[0];
    letter = toupper(letter);

    if (letter < 0 || (size_t)letter >= MORSE_TABLE_MAX) {
      panic("invalid letter in morse table");
    }

    char* morse = &line[2];
    morse_table[(int)letter] = morse;
    string_table[(int)letter] = (morse_string){morse, strlen(morse)};
  }
}

// morse_length gets the length of the string in morse code. Unknown characters
// are ignored for now. The returned length includes the null terminator
// assuming the last character has no space after it.
//
// The function is NOT guaranteed to return the correct length in all cases, but
// it is guaranteed to return a length that is at least as long as the actual
// length.
int morse_length(char* str) {
  int len = 0;
  for (char* s = str; *s != '\0'; s++) {
    if (*s == ' ' || *s == '\n') {
      len += 2;  // overestimate
      continue;
    }

    char* morse = morse_lookup(*s);
    if (morse == NULL) {
      continue;  // ignore for now
    }

    // I forgot that C is bad so strlen is O(n) but whatever.
    // Add 1 for one space between each character (or null terminator).
    len += strlen(morse) + 1;
  }

  return len;
}

char* append(char* out, int out_len, char* str) {
  int len = strlen(str);
  if (len > out_len) {
    panic("unreachable: output buffer too small");
  }
  memcpy(out, str, len);
  return out + len;
}

// morse_encode encodes a string into morse code. The caller is responsible for
// freeing the returned string.
char* string_to_morse(char* input_buf, morse_error_t* error_code) {
  morse_init();

  int morse_buf_len = morse_length(input_buf) + 1;

  char* morse_buf = malloc(morse_buf_len);
  if (morse_buf == NULL) {
    panic("out of memory");
  }

  char* morse_buf_end = morse_buf + morse_buf_len;

  char* curr = morse_buf;
  for (char* s = input_buf; *s != '\0'; s++) {
    char* morse_code;
    bool need_space = false;

    // "three spaces between lines, four spaces between paragraphs" bruh why
    if (strncmp(s, "\n\n", 2) == 0) {
      morse_code = "    ";
      s++;
    } else if (s[0] == '\n') {
      morse_code = "   ";
    } else if (s[0] == ' ') {
      morse_code = "  ";
    } else {
      morse_code = morse_lookup(*s);
      if (morse_code == NULL) {
        *error_code = MORSE_INVALID_CHAR;
        free(morse_buf);
        return NULL;
      }

      // We'll add a space only if the next character is not a space or a
      // newline. Annoying edge case, but whatever.
      need_space = s[1] != ' ' && s[1] != '\n';
    }

    curr = append(curr, morse_buf_end - curr, morse_code);
    if (need_space) {
      curr = append(curr, morse_buf_end - curr, " ");
    }
  }

  // Swap the last space for a null terminator.
  curr[-1] = '\0';

  *error_code = MORSE_NO_ERROR;
  return morse_buf;
}

int skip_spaces(char* str) {
  int i = 0;
  while (str[i] == ' ') {
    i++;
  }
  return i;
}

char* morse_to_string(char* morse_buf, morse_error_t* error_code) {
  morse_init();

  int string_buf_len = strlen(morse_buf) + 1;

  // We're heavily overallocating here, but it's fine. I stopped caring.
  char* string_buf = malloc(string_buf_len);
  if (string_buf == NULL) {
    panic("out of memory");
  }

  char* string_buf_end = string_buf + string_buf_len;

  char* curr_string = string_buf;
  char* curr_morse = morse_buf;
  while (*curr_morse != '\0') {
    int spaces = skip_spaces(curr_morse);
    if (spaces >= 4) {
      // Paragraph break
      curr_string = append(curr_string, string_buf_end - curr_string, "\n\n");
      curr_morse += 4;
      continue;
    }

    if (spaces >= 3) {
      // Line break.
      curr_string = append(curr_string, string_buf_end - curr_string, "\n");
      curr_morse += 3;
      continue;
    }

    if (spaces >= 2) {
      // Two spaces imply a space between words.
      curr_string = append(curr_string, string_buf_end - curr_string, " ");
      curr_morse += 2;
      continue;
    }

    if (spaces == 1) {
      // One space implies a space between characters.
      curr_morse++;
      continue;
    }

    char* end = strchr(curr_morse, ' ');
    int len = end - curr_morse;
    if (end == NULL) {
      len = strlen(curr_morse);
    }

    // tfw when no hash table
    char found_char = 0;
    for (char c = 0; c < MORSE_TABLE_MAX; c++) {
      if (morse_table[(int)c] == NULL) {
        continue;
      }

      morse_string* morse = &string_table[(int)c];
      if (morse->len != len) {
        continue;
      }

      if (strncmp(morse->str, curr_morse, len) == 0) {
        found_char = c;
        break;
      }
    }

    if (found_char == 0) {
      *error_code = MORSE_INVALID_CODE;
      free(string_buf);
      return NULL;
    }

    curr_string[0] = found_char;
    curr_string++;

    curr_morse += len;
  }

  // Swap the last space for a null terminator.
  curr_string[0] = '\0';

  *error_code = MORSE_NO_ERROR;
  return string_buf;
}

char* morse_error_string(morse_error_t error_code) {
  switch (error_code) {
    case MORSE_NO_ERROR:
      return "";
    case MORSE_INVALID_CHAR:
      return "invalid character";
    case MORSE_INVALID_CODE:
      return "invalid morse code";
    default:
      return NULL;
  }
}

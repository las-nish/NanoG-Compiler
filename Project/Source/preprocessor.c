/// preprocessor directive
/// @file : preprocessor.c
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 07:29:43
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

#include "_main.h"

// note:
// not working correctly

// append file
void append(FILE *head, FILE *tail) {
  char buf[BUFSIZ];
  size_t n;
  while ((n = fread(buf, 1, sizeof buf, tail)) > 0)
    if (fwrite(buf, 1, n, head) != n) abort();
  if (ferror(tail)) abort();
}

// preprocessor for file import
void prep_import(void) {
  match_t(k_import);

  if (check_t(t_char_string) == false)
    fatals(current_line, "need character string for imput file", null);

  FILE *file_for_append = fopen(token->value, "r");

  if (file_for_append == null)
    fatals(intnull, "invalid file import / no file", token->value);

  // append(input_file, file_for_append);

  fopen(token->value, "ab");

  match_t(t_char_string);
  return;
}
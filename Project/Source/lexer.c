/// lexical analyzer
/// @file : lexer.c
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 12:27:24
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

#include "_main.h"

#define max_id 512        // maximum length for identifier
#define max_int 512       // maximum length for integer literal
#define max_float 512     // maximum length for floating literal
#define max_string 10240  // maximum length for string literal
#define max_character 1   // maximum length for character literal

// initialize lexical analyzer
void init_lex(void) {
  current_line = 1;
  return;
}

// return token's string
char *token_string(int code) {
  static char *string[] = {
      "+",     "-",      "*",       "/",           "%%",     "++",     "--",
      "==",    "!=",     "<",       ">",           "<=",     ">=",     "&&",
      "||",    "!",      "&",       "|",           "^",      "~",      "<<",
      ">>",    "=",      "+=",      "-=",          "*=",     "/=",     "%%=",
      "<<=",   ">>=",    "&=",      "^=",          "|=",     "size",   "asm",
      "break", "char",   "else",    "empty",       "fn",     "goto",   "if",
      "int",   "import", "link",    "place",       "return", "string", "while",
      "zero",  "(",      ")",       "{",           "}",      "[",      "]",
      ":",     ";",      ".",       ",",           "->",     "$",      "~id",
      "~int",  "~char",  "~string", "~charstring", "~eof"};

  char *ret_str = malloc(strlen(string[code]));
  strcpy(ret_str, string[code]);

  return ret_str;
}

// malloc a new token
static int set_token(int line, int code, char *value) {
  token = calloc(1, sizeof(struct _token));
  if (token == null)
    fatals(intnull, "memory cannot be allocated to the internal process", null);

  token->code = code;
  token->line = line;

  if (value == null)
    token->value = null;
  else {
    token->value = malloc(strlen(value) + 1);
    strcpy(token->value, value);
  }

  if (input_path == null)
    token->file_name = null;
  else {
    token->file_name = malloc(strlen(input_path) + 1);
    strcpy(token->file_name, input_path);
  }
  return 0;
}

// get next character
static int nextc(void) {
  int c = fgetc(input_file);
  return c;
}

// back to previous character
static int prevc(int c) {
  ungetc(c, input_file);
  return c;
}

// skip whitespace(s)
static int skip_space(void) {
  int c;
  c = nextc();

  while (
      c == 0x0009 || c == 0x000B || c == 0x000C || c == 0x0020 || c == 0x00A0 ||
      c == 0x1680 || c == 0x180E || c == 0x2000 || c == 0x2001 || c == 0x2002 ||
      c == 0x2003 || c == 0x2004 || c == 0x2005 || c == 0x2006 || c == 0x2008 ||
      c == 0x2009 || c == 0x200A || c == 0x202F || c == 0x3000 || c == 0x205F ||
      c == 0x000D || c == 0x000A || c == 0x0085 || c == 0x2028 || c == 0x2029) {
    if (c == 0x000A) current_line++;
    c = nextc();
  }

  return c;
}

// ignore comment(s)
static int ignore_comment(int c) {
  if (c == '#') {
    for (;;) {
      if (c == 0x000A || feof(input_file)) {
        if (c == 0x000A) current_line++;
        next_token();
        break;
      }

      c = nextc();
    }
  }

  return 0;
}

// scan hexadecimal integer literal
static int scan_hexad(int c) {
  int length;
  char value[max_int];

  length = 0;
  value[max_int] = intnull;

  value[length++] = '0';
  value[length++] = 'x';

  c = nextc();

  if (!isxdigit(c)) {
    value[length] = intnull;
    fatals(current_line, "invalid hexadecimal integer literal", value);
  }

  while (isxdigit(c)) {
    value[length++] = c;

    if (length >= max_int - 12)
      fatals(current_line, "hexadecimal integer literal is too long", null);

    c = nextc();
  }

  value[length] = intnull;

  set_token(current_line, t_int, value);
  prevc(c);
  return 0;
}

// scan octal integer literal
static int scan_octal(int c) {
  int length;
  char value[max_int];

  length = 0;
  value[max_int] = intnull;

  value[length++] = '0';
  value[length++] = 'o';

  c = nextc();

  if (!isoctal(c)) {
    value[length] = intnull;
    fatals(current_line, "invalid octal integer literal", value);
  }

  while (isoctal(c)) {
    value[length++] = c;

    if (length >= max_int - 12)
      fatals(current_line, "octal integer literal is too long", null);

    c = nextc();
  }

  value[length] = intnull;

  set_token(current_line, t_int, value);
  prevc(c);
  return 0;
}

// scan binary integer literal
static int scan_binary(int c) {
  int length;
  char value[max_int];

  length = 0;
  value[max_int] = intnull;

  value[length++] = '0';
  value[length++] = 'b';

  c = nextc();

  if (!isbinary(c)) {
    value[length] = intnull;
    fatals(current_line, "invalid binary integer literal", value);
  }

  while (isbinary(c) || c == '_') {
    if (c != '_') value[length++] = c;

    if (length >= max_int - 12)
      fatals(current_line, "binary integer literal is too long", null);

    c = nextc();
  }

  value[length] = intnull;

  set_token(current_line, t_int, value);
  prevc(c);
  return 0;
}

// scan integer and float literal
static int scan_digit(int c) {
  int temp_length;
  char value[max_int];

  temp_length = 0;
  value[0] = intnull;

  while (isdigit(c) || c == '_') {
    if (c != '_') value[temp_length++] = c;

    if (temp_length >= max_int - 1)
      fatals(current_line, "integer literal is too long", null);

    c = nextc();
  }

  value[temp_length] = intnull;
  set_token(current_line, t_int, value);

  prevc(c);
  return 0;
}

// scan alpha literal
static int scan_alpha(int c) {
  int length;
  char value[max_id];

  length = 0;
  value[0] = intnull;

  while (isalpha(c) || isdigit(c) || c == '_') {
    value[length++] = c;

    if (length >= max_id - 12)
      fatals(current_line, "identifier is too long", null);

    c = nextc();
  }

  value[length] = intnull;

  if (!strcmp(value, "asm"))
    set_token(current_line, k_asm, null);
  else if (!strcmp(value, "break"))
    set_token(current_line, k_break, null);
  else if (!strcmp(value, "char"))
    set_token(current_line, k_char, null);
  else if (!strcmp(value, "else"))
    set_token(current_line, k_else, null);
  else if (!strcmp(value, "empty"))
    set_token(current_line, k_empty, null);
  else if (!strcmp(value, "fn"))
    set_token(current_line, k_fn, null);
  else if (!strcmp(value, "goto"))
    set_token(current_line, k_goto, null);
  else if (!strcmp(value, "if"))
    set_token(current_line, k_if, null);
  else if (!strcmp(value, "int"))
    set_token(current_line, k_int, null);
  else if (!strcmp(value, "import")) {
    set_token(current_line, k_import, null);

    // import preprocessor
    prep_import();
  } else if (!strcmp(value, "link"))
    set_token(current_line, k_link, null);
  else if (!strcmp(value, "place"))
    set_token(current_line, k_place, null);
  else if (!strcmp(value, "return"))
    set_token(current_line, k_return, null);
  else if (!strcmp(value, "size"))
    set_token(current_line, k_size, null);
  else if (!strcmp(value, "string"))
    set_token(current_line, k_string, null);
  else if (!strcmp(value, "while"))
    set_token(current_line, k_while, null);
  else if (!strcmp(value, "zero"))
    set_token(current_line, k_zero, null);
  else if (!strcmp(value, "_start"))
    fatals(current_line, "this identifier cannot be used in the program",
           "_start");  // because "_start" is the nasm assembly starting point
  else
    set_token(current_line, t_id, value);

  prevc(c);
  return 0;
}

// scan character string
static int scan_char_string(int c) {
  int line;
  int length;
  int string_length;
  char value[max_string];

  line = current_line;
  length = 0;
  string_length = 0;
  value[0] = intnull;

  c = nextc();

  value[length] = intnull;

  for (;;) {
    if (c == '`') break;

    if (c < 0x80 && c > 0xBF)
      fatals(current_line, "not a valid character string literal", null);

    if (string_length >= max_string - 1)
      fatals(current_line, "character string literal is long", null);

    if (c == '\n' || feof(input_file))
      fatals(current_line, "there is no end in the character string literal",
             null);

    value[length++] = c;
    c = nextc();
  }

  value[length++] = intnull;
  set_token(line, t_char_string, value);
  return 0;
}

// check string escape sequence
char *return_escape(int c) {
  c = nextc();

  switch (c) {
    case '_':
      c = nextc();
      if (c == '\n' || c == '\r') {
        if (c == '\r') c = nextc();
        current_line++;
        return "0";
      } else
        fatals(current_line, "there is no end in the character/string literal",
               null);
      break;

    case 'a':
      return "7";

    case 'b':
      return "8";

    case 's':
      return "32";

    case 'f':
      return "12";

    case 'n':
      return "10";

    case 'r':
      return "13";

    case 't':
      return "9";

    case 'v':
      return "11";

    case 'e':
      return "27";

    case '\"':
      return "34";

    case '\'':
      return "39";

    case '\\':
      return "92";

    case '\?':
      return "63";

    case '0':
      return "0";
  }

  return null;
}

// scan string literal
int scan_string(int c) {
  int line;
  int length;
  int string_length;
  char value[max_string];

  line = current_line;
  length = 0;
  string_length = 0;
  value[0] = intnull;

  c = nextc();

  value[length] = intnull;

  for (;;) {
    if (c == '\"') break;

    if (c < 0x80 && c > 0xBF)
      fatals(current_line, "not a valid string literal", null);

    if (c == '\\') {
      char *escape = return_escape(c);
      int escape_length = strlen(escape);
      value[length += escape_length];
      strcat(value, escape);
    } else {
      int char_length = ileng(c);
      value[length += char_length];
      strcat(value, ctoascii(c));
    }

    value[length += 1];
    strcat(value, ",");

    string_length++;

    if (c == '\n' || feof(input_file))
      fatals(current_line, "there is no end in the string literal", null);

    if (string_length >= max_string - 1)
      fatals(current_line, "string literal is long", null);

    c = nextc();
  }

  value[length++] = intnull;
  set_token(line, t_string, value);
  return 0;
}

// scan character literal
int scan_character(int c) {
  c = nextc();

  if (c == '\'')
    fatals(current_line, "invalid character literal", null);
  else if (c == '\\') {
    char *escape = return_escape(c);
    char *new_val = malloc(strlen(escape) + 1);
    strcpy(new_val, escape);
    set_token(current_line, t_char, new_val);
  } else {
    int char_length = ileng(c);
    char *new_val = malloc(char_length + 1);
    strcpy(new_val, ctoascii(c));
    set_token(current_line, t_char, new_val);
  }

  c = nextc();
  if (c != '\'') fatals(current_line, "invalid character literal", null);
  return 0;
}

// goto the next token
int next_token(void) {
  int c;
  c = skip_space();

  switch (c) {
    case EOF:
      set_token(current_line, t_eof, null);
      return 0;

    case '(':
      set_token(current_line, s_lprn, null);
      break;

    case ')':
      set_token(current_line, s_rprn, null);
      break;

    case '{':
      set_token(current_line, s_lcbrk, null);
      break;

    case '}':
      set_token(current_line, s_rcbrk, null);
      break;

    case '[':
      set_token(current_line, s_lbrk, null);
      break;

    case ']':
      set_token(current_line, s_rbrk, null);
      break;

    case ':':
      set_token(current_line, s_col, null);
      break;

    case ';':
      set_token(current_line, s_scol, null);
      break;

    case '.':
      set_token(current_line, s_dot, null);
      break;

    case ',':
      set_token(current_line, s_com, null);
      break;

    case '$':
      set_token(current_line, s_dolar, null);
      break;

    case '+':
      if ((c = nextc()) == '+')
        set_token(current_line, o_incr, null);
      else if (c == '=')
        set_token(current_line, o_pluseq, null);
      else {
        prevc(c);
        set_token(current_line, o_plus, null);
      }

      break;

    case '-':
      if ((c = nextc()) == '-')
        set_token(current_line, o_decr, null);
      else if (c == '=')
        set_token(current_line, o_minuseq, null);
      else if (c == '>')
        set_token(current_line, s_arow, null);
      else {
        prevc(c);
        set_token(current_line, o_minus, null);
      }

      break;

    case '*':
      if ((c = nextc()) == '=') {
        set_token(current_line, o_multieq, null);
      } else {
        prevc(c);
        set_token(current_line, o_multi, null);
      }

      break;

    case '/':
      if ((c = nextc()) == '=')
        set_token(current_line, o_diviseq, null);
      else {
        prevc(c);
        set_token(current_line, o_divis, null);
      }

      break;

    case '%':
      if ((c = nextc()) == '=')
        set_token(current_line, o_moduluseq, null);
      else {
        prevc(c);
        set_token(current_line, o_modulus, null);
      }

      break;

    case '=':
      if ((c = nextc()) == '=') {
        set_token(current_line, o_eqto, null);
      } else {
        prevc(c);
        set_token(current_line, o_eq, null);
      }

      break;

    case '!':
      if ((c = nextc()) == '=') {
        set_token(current_line, o_noteq, null);
      } else {
        prevc(c);
        set_token(current_line, o_not, null);
      }

      break;

    case '<':
      if ((c = nextc()) == '<') {
        int next = nextc();

        if (next == '=')
          set_token(current_line, o_lshifteq, null);
        else {
          prevc(next);
          set_token(current_line, o_lshift, null);
        }
      } else if (c == '=')
        set_token(current_line, o_leseq, null);
      else {
        prevc(c);
        set_token(current_line, o_les, null);
      }

      break;

    case '>':
      if ((c = nextc()) == '>') {
        int next = nextc();

        if (next == '=')
          set_token(current_line, o_rshifteq, null);
        else {
          prevc(next);
          set_token(current_line, o_rshift, null);
        }
      } else if (c == '=')
        set_token(current_line, o_greateq, null);
      else {
        prevc(c);
        set_token(current_line, o_great, null);
      }

      break;

    case '&':
      if ((c = nextc()) == '&')
        set_token(current_line, o_and, null);
      else if (c == '=')
        set_token(current_line, o_bitandeq, null);
      else {
        prevc(c);
        set_token(current_line, o_bitand, null);
      }

      break;

    case '|':
      if ((c = nextc()) == '|')
        set_token(current_line, o_or, null);
      else if (c == '=')
        set_token(current_line, o_inoreq, null);
      else {
        prevc(c);
        set_token(current_line, o_inor, null);
      }

      break;

    case '^':
      if ((c = nextc()) == '=')
        set_token(current_line, o_xoreq, null);
      else {
        prevc(c);
        set_token(current_line, o_xor, null);
      }

      break;

    case '~':
      set_token(current_line, o_unary, null);
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (c == '0') {
        int check_digit = nextc();

        if (check_digit == 'X' || check_digit == 'x') {
          scan_hexad(c);
          break;
        } else if (check_digit == 'O' || check_digit == 'o') {
          scan_octal(c);
          break;
        } else if (check_digit == 'B' || check_digit == 'b') {
          scan_binary(c);
          break;
        } else {
          prevc(check_digit);
        }
      }

      scan_digit(c);
      break;

    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '_':
      scan_alpha(c);
      break;

    case '`':
      scan_char_string(c);
      break;

    case '\"':
      scan_string(c);
      break;

    case '\'':
      scan_character(c);
      break;

    case '@':
      break;

    case '#':
      ignore_comment(c);
      break;

    default:
      fatalc(current_line, "invalid character", c);
      break;
  }

  return 1;
}

// print lexical analyzed token list
void print_lexer(void) {
  while (next_token()) {
    printf("- %-5d | %-20s | %s\n", token->line, token_string(token->code),
           token->value);
  }
}

/* ------- custom function(s) ------- */

// check binary supported character
bool isbinary(int c) {
  if (c == 49 || c == 48) return true;
  return false;
}

// check octal supported character
bool isoctal(int c) {
  if (c == 48 || c == 49 || c == 50 || c == 51 || c == 52 || c == 53 ||
      c == 54 || c == 55)
    return true;
  return false;
}

// length of an integer
int ileng(int c) {
  long long leng = 0;

  while (c != 0) {
    c = c / 10;
    ++leng;
  }

  return leng;
}

// string to digit ( integer )
int stod(char *s) {
  int val = 0;               // Return Value
  int leng = 0;              // Length of the Value
  bool is_negative = false;  // Negative Number
  leng = strlen(s);
  if (s[0] == '-') is_negative = true;
  for (int i = 0; i <= leng - 1; i++) val = val * 10 + (s[i] - '0');
  if (is_negative == true) val = -1 * val;
  return val;
}

// check path
int pathexc(int c) {
  switch (c) {
    case '\"':
    case '*':
    case '\?':
    case ':':
    case '<':
    case '>':
    case '|':
      return 1;

    default:
      return 0;
  }
}

// character to string
char *ctos(int c) {
  char *ret_val;
  ret_val = malloc(1);
  ret_val[0] = c;
  return ret_val;
}

// digit to string
char *dtos(int c) {
  char *ret_val;
  ret_val = malloc(ileng(c) + 1);
  sprintf(ret_val, "%d", c);
  return ret_val;
}

// character to ascii
char *ctoascii(int c) {
  char *return_ascii;
  return_ascii = malloc(ileng(c) + 1);
  sprintf(return_ascii, "%d", c);
  return return_ascii;
}

// copy string
char *copystr(char *s) {
  if (s == null) return null;

  int get_leng = strlen(s) + 1;
  char *rets = malloc(get_leng);
  strcpy(rets, s);

  return rets;
}
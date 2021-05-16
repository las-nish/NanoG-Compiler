/// main header
/// @file : _main.h
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 12:16:12
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define null NULL
#define intnull '\0'

#define xxx printf("xxx\n");

/* ------- for global variable ------- */

char* compiler_path;  // path of the compiler

/* ------- for lexical analyzer ------- */

FILE* input_file;   // pointer for opened file
FILE* output_file;  // pointer for created file
char* input_path;   // path of the running file
char* output_path;  // path of the generated file
int current_line;   // current line number
struct _token {     // token's structure
  int code;         // token code
  int line;         // line number of the token
  char* value;      // value for token
  char* file_name;  // file name that contains the symbol
} * token;

/* ------- for syntax analyzer ------- */

// flags to find current locations
enum {
  loc_global,  // in global area
  loc_block,   // in block
  loc_loop,    // in loop block
  loc_param    // in function parameter
};

int location;   // for setup current location
int offset;     // offset for increase in bytes
char* fn_name;  // function name for place and goto statements

bool is_return;   // is return available in function
bool is_program;  // is it program function (starting function)

struct _expr {          // for generate expression and emit data
  int op;               // expression operation
  char* value;          // value
  struct _expr* left;   // left node
  struct _expr* right;  // right node
};

/* ------- for symbol table ------- */

// storage class
#define stc_macro 10  // macro
#define stc_asm 20    // inline assembly data
#define stc_link 30   // link
#define stc_fn 40     // function
#define stc_fnpro 41  // function prototype
#define stc_var 50    // variable
#define stc_place 60  // place

// data type
#define dt_none 10   // for assembly and link
#define dt_int 2     // 64bit integer
#define dt_char 3    // 8bit character
#define dt_string 4  // 64bit string

// short definition
#define sym_list struct _symbol_list
#define symbol struct _symbol
#define scope struct _scope

int sym_id;  // current id of the current symbol

struct _symbol_list {
  int id;                     // id of the symbol
  char* name;                 // name of the symbol
  uint64_t hash;              // hash index of the symbol
  int storage;                // storage class of the symbol
  struct _symbol_list* next;  // next symbol
  struct _scope* body;        // body scope
};

struct _symbol {         // symbol table
  int id;                // id of the symbol
  char* name;            // symbol name
  char* value;           // symbol value
  int storage;           // storage class
  int data;              // data type
  int size;              // data size in bytes
  int offset;            // offset in bytes
  struct _symbol* next;  // next symbol
} * table;               // table node

struct _scope {
  int count_scope;            // all symbols value
  struct _symbol_list* list;  // symbol list
  struct _scope* next;        // next scope
};

scope* glob_scope;  // global scope structure
scope* temp_scope;  // temporary structure for store global scope

/* ------- for code generator ------- */

struct _symbol* current_symbol;  // current symbol data for variable emit, to
                                 // get details like offset
int break_code;                  // label code for jump to break
int parent;     // parent keyword (if or while - for code generation)
int cur_label;  // for store current jump label

// custom header file list
#include "_function.h"
#include "_token.h"
/// function header
/// @file : _function.h
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 12:16:39
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

#ifndef _HEADER_FUNCTION_
#define _HEADER_FUNCTION_

// function prototypes in "lexer.c"

void init_lex(void);
char* token_string(int code);
int next_token(void);
void print_lexer(void);
bool isbinary(int c);
bool isoctal(int c);
int ileng(int c);
int stod(char* s);
int pathexc(int c);
char* dtos(int c);
char* ctoascii(int c);
char* copystr(char* s);

// function prototypes in "main.c"

void init_global(void);
char* set_file_ext(char* open, char* ext);
void fatalc(int line, char* message, int c);
void fatald(int line, char* message, int c);
void fatals(int line, char* message, char* s);

// function prototypes in "nasm.c"

void add_bss(char* body);
void add_data(char* body);
int free_reg_param(void);
void freeall_reg(void);
void asm_file_begin(void);
void asm_file_end(void);
void emit_comment(char* body);
void emit_inline_asm(char* body);
void emit_fn_name(char* name);
void emit_fn_end(void);
void emit_var(int data_type, int offset, char* value);
void emit_var_data(int data_type, char* name, char* value);
void emit_var_para(int data_type, int offset, char* name);
void emit_place(char* name, char* fn_name);
void emit_goto(char* name, char* fn_name);
int emit_if(void);
int emit_if_else(int val);
int emit_end_if_else(int val);
int emit_while(void);
int emit_while_end(int val);
void emit_break(void);
void emit_return(int reg);

struct _expr* malloc_expr(int op, char* val, struct _expr* left,
                          struct _expr* right);
int invert_expr(struct _expr* node);
int gen_expr(struct _expr* temp);

// function prototypes in "parser.c"

void init_parser(void);
int match_t(int code);
bool check_t(int code);
int program(void);

// function prototypes in "preprocessor.c"

void prep_import(void);

// function prototypes in "symbol.c"

void init_symbol(void);
void scope_enter(void);
int scope_exit(void);
void scope_sub_enter(sym_list* sym);
int scope_sub_exit(void);
int scope_count(scope* s);
bool lookup_cur(char* name);
sym_list* lookup(char* name);
sym_list* lookup_sub(scope* s, char* name);
uint64_t hash_fnv(char* s);
void hash_fnv_test(void);
static sym_list* malloc_list(int id, char* name, int storage,
                             struct _symbol_list* next, struct _scope* body);
sym_list* add_list(char* name, int storage);
symbol* add_symbol(int id, char* name, char* value, int storage, int data,
                   int size, int offset);
symbol* return_table(int id);
void test_symbol(void);

#endif
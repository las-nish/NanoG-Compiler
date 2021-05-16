/// symbol table
/// @file : symbol.c
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 12:10:15
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

// note:
// 1. only one symbol table for whole program
// 2. no scope support

#include "_main.h"

// initialize symbol table
void init_symbol(void) {
  sym_id = 0;
  return;
}

/* ------- scope management and symbol checking ------- */

// enter to scope
void scope_enter(void) {
  scope* s = calloc(1, sizeof(scope));
  s->next = glob_scope;
  glob_scope = s;
}

// leave from scope
int scope_exit(void) {
  if (glob_scope->next == null) {
    // printf("methnin ub ayinweyi puka\n");
    return -1;
  }

  glob_scope = glob_scope->next;

  return 0;
}

// enter to a sub scope
void scope_sub_enter(sym_list* sym) {
  sym->body = calloc(1, sizeof(scope));
  sym->body->next = glob_scope;
  glob_scope = sym->body;
}

// leave from a sub scope
int scope_sub_exit(void) {
  if (glob_scope->next == null) return -1;

  glob_scope = glob_scope->next;
  return 0;
}

// count symbols in the scope
int scope_count(scope* s) {
  sym_list* temp = s->list;
  int ret_count = 0;

  if (temp == null) return 0;

  while (temp != null) {
    ret_count += 1;
    temp = temp->next;
  }

  free(temp);

  return ret_count;
}

// lookup symbol in current scope
bool lookup_cur(char* name) {
  if (glob_scope == null) return false;

  sym_list* s = glob_scope->list;
  uint64_t hash = hash_fnv(name);
  int count = scope_count(glob_scope);

  for (int i = 0; i < count; i++) {
    if (s->hash == hash) {
      return true;
    }

    s = s->next;
  }

  return false;
}

// lookup and return given symbol's structure from current scope to the global
// scope scanning
sym_list* lookup(char* name) {
  bool is_found = false;
  sym_list* ret_symbol = null;

  // store global scope to the temporary scope
  temp_scope = calloc(1, sizeof(glob_scope));
  temp_scope = glob_scope;

lookup_next:
  ret_symbol = glob_scope->list;
  uint64_t hash = hash_fnv(name);
  int count = scope_count(glob_scope);

  for (int i = 0; i < count; i++) {
    if (ret_symbol->hash == hash) {
      is_found = true;
      break;
    }

    ret_symbol = ret_symbol->next;
  }

  if (is_found == false) {
    int check_scope_exit = scope_exit();
    if (check_scope_exit != -1)
      goto lookup_next;
    else
      goto lookup_end;
  }

lookup_end:
  // re-store global scope structure from the temporary scope
  glob_scope = calloc(1, sizeof(temp_scope));
  glob_scope = temp_scope;

  return ret_symbol;
}

// lookup and return given symbol's structure from given scope (this used for
// sub scope scanning)
sym_list* lookup_sub(scope* s, char* name) {
  bool is_found = false;
  sym_list* ret_symbol = null;

  if (s == null) return false;

  ret_symbol = s->list;
  uint64_t hash = hash_fnv(name);
  int count = scope_count(s);

  for (int i = 0; i < count; i++) {
    if (ret_symbol->hash == hash) {
      is_found = true;
      break;
    }

    ret_symbol = ret_symbol->next;
  }

  if (is_found == false) return false;

  return ret_symbol;
}

/* ------- hash function ------- */

// "fnv-1" hash algorithm
uint64_t hash_fnv(char* s) {
  if (s == null) return 0;

  int key_leng = strlen(s);
  uint64_t hash_index = 0xcbf29ce484222325;

  for (int i = 0; i < key_leng; i++) {
    hash_index = hash_index * 0x100000001b3;
    hash_index = hash_index ^ (unsigned char)s[i];
  }

  return hash_index;
}

// test "hash_fnv" hash function
void hash_fnv_test(void) {
  printf("- fnv hash function test -\n");
  printf(" > hash key for %s : %ld\n", "Lasan", hash_fnv("Lasan"));
  printf(" > hash key for %s : %ld\n", "Gaweshani", hash_fnv("Gaweshani"));
  printf(" > hash key for %s : %ld\n", "Mantha", hash_fnv("Mantha"));
  printf(" > hash key for %s : %ld\n", "Dhammachintha",
         hash_fnv("Dhammachintha"));
  printf(" > hash key for %s : %ld\n", "Nishshanka", hash_fnv("Nishshanka"));
  printf(" > hash key for %s : %ld\n", "Rajapakshe", hash_fnv("Rajapakshe"));
}

/* ------- symbol list ------- */

// maloc new symbol to the symbol list structure
static sym_list* malloc_list(int id, char* name, int storage,
                             struct _symbol_list* next, struct _scope* body) {
  sym_list* s = (sym_list*)malloc(sizeof(sym_list));
  if (s == null)
    fatals(intnull, "unable to malloc symbol list structure", null);

  if (name == null)
    s->name = null;
  else {
    s->name = malloc(strlen(name + 1));
    strcpy(s->name, name);
  }

  s->hash = hash_fnv(name);
  s->id = id;
  s->storage = storage;
  s->next = next;
  s->body = body;

  return s;
}

// add new symbol to the current symbol list structure
sym_list* add_list(char* name, int storage) {
  int id;
  sym_list* sym;

  sym_id++;
  id = sym_id;

  sym = calloc(1, sizeof(sym_list));
  sym = malloc_list(id, name, storage, null, null);

  if (glob_scope == null) {
    // return fatals "scope ekakt enter weyan huththo!"
    fatals(intnull, "global scope is empty", null);
  } else {
    if (glob_scope->list == null) {
      glob_scope->list = sym;

      glob_scope->count_scope = scope_count(glob_scope);
    } else {
      sym_list* s = glob_scope->list;
      while (s->next != null) s = s->next;
      s->next = sym;

      glob_scope->count_scope = scope_count(glob_scope);
    }
  }

  return sym;
}

/* ------- symbol table ------- */

// malloc symbol
static symbol* malloc_symbol(int id, char* name, char* value, int storage,
                             int data, int size, int offset, symbol* next) {
  symbol* temp = (symbol*)malloc(sizeof(symbol));

  temp->id = id;

  if (name == null)
    temp->name = null;
  else {
    temp->name = malloc(strlen(name) + 1);
    strcpy(temp->name, name);
  }

  if (value == null)
    temp->value = null;
  else {
    temp->value = malloc(strlen(value) + 1);
    strcpy(temp->value, value);
  }

  temp->storage = storage;
  temp->data = data;
  temp->size = size;
  temp->offset = offset;
  temp->next = next;

  return temp;
}

// add new symbol to the current table
symbol* add_symbol(int id, char* name, char* value, int storage, int data,
                   int size, int offset) {
  if (table == null)
    return (table = malloc_symbol(id, name, value, storage, data, size, offset,
                                  null));
  else {
    symbol* temp = table;
    while (temp->next != null) temp = temp->next;
    return (temp->next = malloc_symbol(id, name, value, storage, data, size,
                                       offset, null));
  }

  return null;
}

// return node of given symbol table item abbording to the id
symbol* return_table(int id) {
  bool is_found = false;
  symbol* ret_symbol = table;

  if (ret_symbol == NULL) return NULL;

  while (ret_symbol != NULL) {
    if (ret_symbol->id == id) {
      is_found = true;
      break;
    }

    ret_symbol = ret_symbol->next;
  }

  if (is_found == false) return NULL;

  return ret_symbol;
}

/* ------- test symbol ------- */

// print symbol in curent scope
static void print_test_scope(void) {
  // note:
  // this function change the current node of global scope

  if (glob_scope == NULL)
    fatals(intnull, "null global scope in symbol table", null);

  while (glob_scope->list != NULL) {
    printf(" -- %d | %s\n", glob_scope->count_scope, glob_scope->list->name);
    glob_scope->list = glob_scope->list->next;
  }
}

// test symbol table
void test_symbol(void) {
  // symbol_1;                     -> global scope
  // symbol_2;
  // symbol_3;
  // symbol_4;
  //
  // fn {                          -> function scope
  //   symbol_11;
  //   symbol_12;
  //   symbol_13;
  //   symbol_14;
  //
  //   if {                        -> scope inside function
  //     symbol_111;
  //     symbol_112;
  //   }
  //
  //   symbol_15;
  //   symbol_16;
  // }
  //
  // symbol_5;
  // symbol_6;
  //
  // struct symbol_7 {             -> structure scope
  //   sub_symbol_1;
  //   sub_symbol_2;
  //   sub_symbol_3;
  //   sub_symbol_4;
  //
  //   struct symbol_71 {           -> structure scope inside structure scope
  //     sub_symbol_11;
  //     sub_symbol_22;
  //   }
  // }
  //
  // symbol_8;
  // symbol_9;
  // symbol_10;

  scope_enter();  // enter to global scope

  add_list("symbol_1", 0);
  add_list("symbol_2", 0);
  add_list("symbol_3", 0);
  add_list("symbol_4", 0);

  /****/ scope_enter();  // enter to function scope
  /****/ add_list("symbol_11", 1);
  /****/ add_list("symbol_12", 1);
  /****/ add_list("symbol_13", 1);
  /****/ add_list("symbol_14", 1);
  /***********/ scope_enter();  // enter to scope inside function
  /***********/ add_list("symbol_111", 2);
  /***********/ add_list("symbol_112", 2);
  /***********/
  /***********/ /* ----> print symbols in the scope inside function */
  /***********/ printf(">> symbols in the scope inside function\n");
  /***********/ print_test_scope();
  /***********/
  /***********/ scope_exit();  // exit from scope inside function
  /****/ add_list("symbol_15", 1);
  /****/ add_list("symbol_16", 1);
  /****/
  /****/ /* ----> print symbols in the function scope */
  /****/ printf(">> symbols in the function scope\n");
  /****/ print_test_scope();
  /****/
  /****/ scope_exit();  // exit from function scope

  add_list("symbol_5", 0);
  add_list("symbol_6", 0);

  /****/ sym_list* st = add_list("symbol_7", 0);
  /****/ scope_sub_enter(st);  // enter to structure
  /****/ add_list("sub_symbol_1", 1);
  /****/ add_list("sub_symbol_2", 1);
  /****/ add_list("sub_symbol_3", 1);
  /****/ add_list("sub_symbol_4", 1);
  /***********/ sym_list* st_inside = add_list("symbol_71", 0);
  /***********/ scope_sub_enter(st_inside);  // enter to sub structure
  /***********/ add_list("sub_symbol_11", 2);
  /***********/ add_list("sub_symbol_22", 2);
  /***********/ scope_sub_exit();  // exit from sub structure
  /****/ scope_sub_exit();         // exit from structure

  add_list("symbol_8", 0);
  add_list("symbol_9", 0);
  add_list("symbol_10", 0);

  /* ----> lookup symbol in current scope and get data of that */
  sym_list* get_01 = lookup("symbol_7");
  printf("+ %s found in global scope\n", get_01->name);
  sym_list* get_sub_01 = lookup_sub(get_01->body, "sub_symbol_4");
  if (get_sub_01 == NULL)
    printf("+ symbol not found in sub scope\n");
  else
    printf("+ symbol %s (%ld) found on sub scope\n", get_sub_01->name,
           get_sub_01->hash);

  /* ----> lookup symbol in current scope */
  if (lookup_cur("symbol_1") == true)
    printf("+ symbol_1 found in global scope\n");
  else
    printf("+ symbol_1 not found in global scope\n");

  /* ----> print symbols in the global scope */
  printf(">> symbols in the global scope\n");
  print_test_scope();

  scope_exit();  // exit from global scope
}
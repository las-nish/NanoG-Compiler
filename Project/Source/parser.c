/// parser
/// @file : parser.c
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 01:00:42
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

// note:
// 1. this is a single parser
// 2. nasm assembly code is generate together with parsing
// 3. main function is "program"

#include "_main.h"

// function prototype
static int program_body(void);
static int asm_decl(void);
static int asm_data_init(void);
static int asm_end(void);
static int asm_body_item(void);
static int link_decl(void);
static int fn_decl(void);
static int fn_param(void);
static int block(void);
static int var_decl(bool need_start);
static int stmt(void);
static int if_stmt(void);
static int while_stmt(void);
static int break_stmt(void);
static int place_stmt(void);
static int goto_stmt(void);
static int return_stmt(void);
static int expr_stmt(void);
static struct _expr* expr(void);
static struct _expr* assignment_expr(void);
static struct _expr* cond_expr(void);
static struct _expr* logical_or_expr(void);
static struct _expr* logical_and_expr(void);
static struct _expr* inclusive_or_expr(void);
static struct _expr* exclusive_or_expr(void);
static struct _expr* and_expr(void);
static struct _expr* equality_expr(void);
static struct _expr* relational_expr(void);
static struct _expr* shift_expr(void);
static struct _expr* addictive_expr(void);
static struct _expr* multiplicative_expr(void);
static struct _expr* unary_expr(void);
static struct _expr* postfix_expr(void);
static struct _expr* primary_expr(void);
static int data_type(int code);
static int ret_data_size(int code);

// initialize parser
void init_parser(void) {
  location = 0;
  offset = 0;
  fn_name = null;
}

// match next token
int match_t(int code) {
  if (token->code == code)
    return next_token();
  else {
    fatals(current_line, "expected token", token_string(code));
    return next_token();
  }
}

// check next token
bool check_t(int code) {
  if (token->code == code)
    return true;
  else
    return false;
}

// starting symbol
int program(void) {
  next_token();
  scope_enter();          // enter to global scope
  location = loc_global;  // global location
  if (token->code >= 0) return program_body();
  location = loc_global;  // global location
  scope_exit();           // exit from global scope
  return 0;
}

// program_body = asm_decl | link_decl | fn_decl | var_decl
static int program_body(void) {
  int code = 0;  // current token
  int line = 0;  // token's line

  for (;;) {
    code = token->code;
    line = token->line;

    switch (code) {
      case t_eof:  // ignore this
        break;     // end of the program

      case k_asm:
        asm_decl();
        break;

      case k_link:
        link_decl();
        break;

      case k_fn:
        fn_decl();
        break;

      case k_int:
      case k_char:
      case k_string:
        var_decl(false);
        break;

      default:
        fatals(line, "invalid token", token_string(code));
        break;
    }

    if (code == t_eof) break;
  }

  return 0;
}

// asm_decl = 'asm' asm_data_init '=' asm_end
static int asm_decl(void) {
  match_t(k_asm);
  if (check_t(s_col) == true) asm_data_init();
  match_t(o_eq);
  asm_end();

  return 0;
}

// asm_data_init = ':' identifier ( ',' identifier )*
static int asm_data_init(void) {
  char* name = null;  // name

  match_t(s_col);

get_next_name:
  if (check_t(t_id) == false)
    fatals(current_line, "need an identifier for assembly data initialize",
           token_string(token->code));

  name = malloc(strlen(token->value) + 1);
  strcpy(name, token->value);

  // lookup symbol
  if (lookup_cur(name) == true)
    fatals(current_line, "symbol already defined", name);

  // add symbol
  sym_list* s_item = add_list(name, stc_asm);
  add_symbol(s_item->id, s_item->name, null, s_item->storage, dt_none, intnull,
             intnull);

  match_t(t_id);

  if (check_t(s_com) == true) {
    match_t(s_com);
    name = null;
    goto get_next_name;
  }

  return 0;
}

// asm_end = '[' asm_body ']'
// asm_body = asm_body_item ( ',' asm_body_item )*
static int asm_end(void) {
  match_t(s_lbrk);

get_next_item:
  asm_body_item();

  if (check_t(s_com) == true) {
    match_t(s_com);
    goto get_next_item;
  }

  match_t(s_rbrk);

  return 0;
}

// asm_body_item = asm_item ( '+' asm_item )*
// asm_item = char_string_literal | identifier
static int asm_body_item(void) {
  char get_line[2048];  // get input's value
  char set_line[2048];  // set input's value

  get_line[0] = intnull;
  set_line[0] = intnull;

get_next_part:
  if (strlen(get_line) >= 2048 - 48 || strlen(set_line) >= 2048 - 48)
    fatald(current_line, "inline assembly input line is too long",
           strlen(get_line));

  if (check_t(t_char_string) == true) {
    if (token->value != null) {
      snprintf(get_line, strlen(token->value) + 1, "%s", token->value);
      strcat(set_line, get_line);
    }

    match_t(t_char_string);
  } else if (check_t(t_id) == true || strlen(set_line) >= 2048 - 48) {
    // check token in symbol list and if available get it's table node
    sym_list* s_item = lookup(token->value);
    if (s_item == null)
      fatals(current_line, "no symbol defined for use in inline assembly",
             token->value);
    symbol* sym = return_table(s_item->id);

    if (sym->value != null) {
      snprintf(get_line, strlen(sym->value) + 1, "%s", sym->value);
      strcat(set_line, get_line);
    } else
      fatals(current_line, "cannot get value from given identifier", null);

    match_t(t_id);
  } else
    fatals(current_line, "invalid inline assembly line input",
           token_string(token->code));

  if (check_t(o_plus) == true) {
    match_t(o_plus);
    goto get_next_part;
  }

  emit_inline_asm(set_line);
  return 0;
}

// link_decl = 'link' identifier ';'
static int link_decl(void) {
  char* name = null;  // link's name

  match_t(k_link);

  if (check_t(t_id) == false)
    fatals(current_line, "need an identifier for link name",
           token_string(token->code));

  name = malloc(strlen(token->value) + 1);
  strcpy(name, token->value);

  // lookup symbol
  if (lookup_cur(name) == true)
    fatals(current_line, "symbol already defined", name);

  // add symbol
  sym_list* s_item = add_list(name, stc_link);
  add_symbol(s_item->id, s_item->name, null, s_item->storage, dt_none, intnull,
             intnull);

  match_t(t_id);
  match_t(s_scol);
  return 0;
}

// fn_decl = fn_start fn_end
// fn_start = 'fn' identifier
// fn_type = '->' data type
// fn_end = ';' | fn_param fn_type? block
static int fn_decl(void) {
  char* name = null;  // function's name
  int type = 0;       // function's data type

  is_return = false;  // set return available in function to false (default)
  free_reg_param();   // free registers for function parameters

  match_t(k_fn);

  if (check_t(t_id) == false)
    fatals(current_line, "need an identifier for function name",
           token_string(token->code));

  name = malloc(strlen(token->value) + 1);
  strcpy(name, token->value);

  // check is it program function (starting function) or not
  if (!strcmp(name, "program"))
    is_program = true;
  else
    is_program = false;

  // lookup symbol
  sym_list* look_s_item = lookup(name);
  if (look_s_item != null) {
    symbol* sym = return_table(look_s_item->id);
    if (sym->storage != stc_fnpro)
      fatals(current_line, "symbol already defined", name);
  }

  // add symbol
  sym_list* s_item = add_list(name, stc_fnpro);
  symbol* sym = add_symbol(s_item->id, s_item->name, null, s_item->storage,
                           dt_none, intnull, intnull);

  // set function name
  fn_name = malloc(strlen(name) + 1);
  strcpy(fn_name, name);

  match_t(t_id);

  if (check_t(s_scol) == true) {
    match_t(s_scol);
    return 0;
  }

  // emit function start
  emit_fn_name(name);

  scope_enter();         // enter to scope
  location = loc_block;  // block location
  offset = 0;            // set global offset to zero

  location = loc_param;  // parameter location
  fn_param();
  location = loc_block;  // return to block location

  // set storage class to function instead of function prototype
  s_item->storage = stc_fn;

  if (check_t(s_arow) == true) {
    match_t(s_arow);
    type = data_type(token->code);
    sym->data = type;
  } else {
    sym->data = dt_int;
  }

  block();

  // set function name too null
  fn_name = null;

  // emit function end
  emit_fn_end();

  offset = 0;             // set global offset to zero
  location = loc_global;  // return to global location
  scope_exit();           // exit from scope

  return 0;
}

// fn_param = '(' param_item_list ')'
// param_item_list = param_item ( ',' param_item )*
// param_item = var_start
static int fn_param(void) {
  match_t(s_lprn);

  if (check_t(s_rprn) == true) {
    match_t(s_rprn);
    return 0;
  }

get_next_param:
  var_decl(true);

  if (check_t(s_com) == true) {
    match_t(s_com);
    goto get_next_param;
  }

  match_t(s_rprn);

  return 0;
}

// block = '{' block_body '}'
// block_body = block_item*
// block_item = asm_decl | var_decl | stmt
static int block(void) {
  int code = 0;  // current token

  scope_enter();
  match_t(s_lcbrk);

  for (;;) {
    code = token->code;

    switch (code) {
      case s_rcbrk:  // ignore
        break;

      case k_asm:
        asm_decl();
        break;

      case k_int:
      case k_char:
      case k_string:
        var_decl(false);
        break;

      default:
        stmt();
        break;
    }

    if (code == s_rcbrk) break;
  }

  match_t(s_rcbrk);
  scope_exit();

  return 0;
}

// var_decl = var_start '=' var_end
// var_start = data_type identifier
// var_end = literal ';'
static int var_decl(bool need_start) {
  int data = 0;        // data type of the variable
  char* name = null;   // variable's name
  char* value = null;  // variable's value

  data = data_type(token->code);

  if (check_t(t_id) == false)
    fatals(current_line, "need an identifier for variable name",
           token_string(token->code));

  name = malloc(strlen(token->value) + 1);
  strcpy(name, token->value);

  // lookup symbol
  if (lookup_cur(name) == true)
    fatals(current_line, "symbol already defined", name);

  // add symbol
  sym_list* s_item = add_list(name, stc_var);
  symbol* sym = add_symbol(s_item->id, s_item->name, null, s_item->storage,
                           data, ret_data_size(data), intnull);

  // update global offset size
  offset += ret_data_size(data);

  match_t(t_id);

  if (need_start == false) {
    match_t(o_eq);

    if (data == dt_int) {
      if (check_t(t_int) == false)
        fatals(current_line, "need an integer literal",
               token_string(token->code));

      value = malloc(strlen(token->value) + 1);
      strcpy(value, token->value);

      // update symbol
      sym->value = value;

      match_t(t_int);
    } else if (data == dt_string) {
      if (check_t(t_string) == false)
        fatals(current_line, "need an string literal",
               token_string(token->code));

      value = malloc(strlen(token->value) + 1);
      strcpy(value, token->value);

      // update symbol
      sym->value = value;

      match_t(t_string);
    } else if (data == dt_char) {
      if (check_t(t_char) == true) {
        if (check_t(t_char) == false)
          fatals(current_line, "need an integer/character literal",
                 token_string(token->code));

        value = malloc(strlen(token->value) + 1);
        strcpy(value, token->value);

        // update symbol
        sym->value = value;

        match_t(t_char);
      } else {
        if (check_t(t_int) == false)
          fatals(current_line, "need an integer/character literal",
                 token_string(token->code));

        value = malloc(strlen(token->value) + 1);
        strcpy(value, token->value);

        // update symbol
        sym->value = value;

        match_t(t_int);
      }
    }

    sym->offset = offset;

    // emit code according to the location
    if (location == loc_global) {
      // ? setup node
      current_symbol = null;                               // reset
      current_symbol = calloc(1, sizeof(struct _symbol));  // calloc size
      current_symbol = sym;                                // allocate symbol

      emit_var_data(data, sym->name, sym->value);
    } else if (location == loc_block || location == loc_loop) {
      // ? setup node
      current_symbol = null;                               // reset
      current_symbol = calloc(1, sizeof(struct _symbol));  // calloc size
      current_symbol = sym;                                // allocate symbol

      emit_var(data, sym->offset, sym->value);
    }

    match_t(s_scol);
  } else {
    if (location == loc_param) {
      // ? setup node
      current_symbol = null;                               // reset
      current_symbol = calloc(1, sizeof(struct _symbol));  // calloc size
      current_symbol = sym;                                // allocate symbol

      sym->offset = offset;
      emit_var_para(data, sym->offset, sym->name);
    }
  }

  return 0;
}

/* ------- statement ------- */

// stmt = if_stmt | while_stmt | break_stmt | place_stmt | goto_stmt
//      | break_stmt | continue_stmt |  return_stmt | expr_stmt
static int stmt(void) {
  int code = 0;  // current token
  code = token->code;

  switch (code) {
    case k_if:
      return if_stmt();

    case k_while:
      return while_stmt();

    case k_break:
      return break_stmt();

    case k_place:
      return place_stmt();

    case k_goto:
      return goto_stmt();

    case k_return:
      return return_stmt();

    default:
      return expr_stmt();
  }

  return 0;
}

// if_stmt = 'if' cond_expr block ( 'else' block )?
static int if_stmt(void) {
  match_t(k_if);

  //        perform the opposite comparison
  //        jump to L0 if true
  //        perform the first block of code
  //        jump to L1
  // L0:
  //        perform the other block of code
  // L1:

  int label_1 = 0;

  parent = k_if;  // set parent to IF statement

  label_1 = emit_if();

  // ? jump after expression (label_1 - 2)
  cur_label = label_1 - 2;

  freeall_reg();                     // ? free all registers
  struct _expr* temp = cond_expr();  // get conditional expression
  gen_expr(temp);                    // generate expression
  parent = 0;                        // reset parent to zero

  block();

  if (check_t(k_else) == true) {
    emit_if_else(label_1 - 1);

    match_t(k_else);
    block();
  }

  emit_end_if_else(label_1);

  return 0;
}

// while_stmt = 'while' cond_expr block
static int while_stmt(void) {
  match_t(k_while);

  int label_1 = 0;

  parent = k_while;                  // set parent to WHILE statement
  struct _expr* temp = cond_expr();  // get conditional expression

  label_1 = emit_while();

  location = loc_loop;  // set location to loop block

  block();

  location = loc_block;  // reset location to loop block

  emit_while_end(label_1);

  // ? jump after expression (label_1 + 1)
  cur_label = label_1 + 1;
  freeall_reg();   // ? free all registers
  gen_expr(temp);  // generate expression

  parent = 0;  // reset parent to zero

  return 0;
}

// break_stmt = 'break' ';'
static int break_stmt(void) {
  match_t(k_break);

  if (location != loc_loop)
    fatals(current_line, "unable to use break statement inside non-loop block",
           null);

  // emit break
  emit_break();

  match_t(s_scol);

  return 0;
}

// place_stmt = 'place' identifier ':'
static int place_stmt(void) {
  char* name = null;  // place's name

  match_t(k_place);

  if (check_t(t_id) == false)
    fatals(current_line, "need an identifier for place name",
           token_string(token->code));

  name = malloc(strlen(token->value) + 1);
  strcpy(name, token->value);

  // lookup symbol
  if (lookup_cur(name) == true)
    fatals(current_line, "symbol already defined", name);

  // add symbol
  sym_list* s_item = add_list(name, stc_place);
  add_symbol(s_item->id, s_item->name, null, s_item->storage, dt_none, intnull,
             intnull);

  match_t(t_id);
  match_t(s_col);

  // emit place
  emit_place(name, fn_name);
  return 0;
}

// goto_stmt = 'goto' identifier ';'
static int goto_stmt(void) {
  char* name = null;  // goto's name

  match_t(k_goto);

  if (check_t(t_id) == false)
    fatals(current_line, "need an identifier for check place statement",
           token_string(token->code));

  name = malloc(strlen(token->value) + 1);
  strcpy(name, token->value);

  // lookup symbol
  if (lookup_cur(name) == false) fatals(current_line, "no symbol found", name);

  // check is it place symbol
  sym_list* s_item = lookup(name);
  if (s_item->storage != stc_place)
    fatals(current_line, "need place symbol for use goto statement", name);

  match_t(t_id);
  match_t(s_scol);

  // emit goto
  emit_goto(name, fn_name);
  return 0;
}

// return_stmt = 'return' expr? ';'
static int return_stmt(void) {
  match_t(k_return);

  is_return = true;  // set is return available in function to true

  freeall_reg();                // ? free all registers
  struct _expr* temp = expr();  // get expression

  // reg integer variable is used to found which fucking register number needs
  // to return
  int reg = gen_expr(temp);  // generate expression

  // emit return
  emit_return(reg);

  match_t(s_scol);
  return 0;
}

// expr_stmt = expr ';'
static int expr_stmt(void) {
  freeall_reg();                // ? free all registers
  struct _expr* temp = expr();  // get expression
  gen_expr(temp);               // generate expression
  match_t(s_scol);

  return 0;
}

/* ------- expression ------- */

// expr = assignment_expr
static struct _expr* expr(void) { return assignment_expr(); }

// assignment_expr = cond_expr ( ( '=' | '+=' | '-=' | '*=' | '/=' | '%=' |
// '<<=' | '>>=' | '&=' | '^=' | '|=' ) assignment_expr )?
static struct _expr* assignment_expr(void) {
  struct _expr* node = NULL;       // main node
  struct _expr* cond_node = NULL;  // conditional expression
  int code = 0;                    // current token

  cond_node = cond_expr();

  code = token->code;

  switch (code) {
    case o_eq:         // "="
    case o_pluseq:     // "+="
    case o_minuseq:    // "-="
    case o_multieq:    // "*="
    case o_diviseq:    // "/="
    case o_moduluseq:  // "%="
    case o_lshifteq:   // "<<="
    case o_rshifteq:   // ">>="
    case o_bitandeq:   // "&="
    case o_xoreq:      // "^="
    case o_inoreq:     // "|="
      match_t(code);
      node = malloc_expr(code, null, cond_node, assignment_expr());
      break;

    default:
      node = cond_node;
      break;
  }

  return node;
}

// cond_expr = logical_or_expr
static struct _expr* cond_expr(void) { return logical_or_expr(); }

// logical_or_expr = logical_and_expr ( '||' logical_or_expr )?
static struct _expr* logical_or_expr(void) {
  struct _expr* node = NULL;          // main node
  struct _expr* log_and_node = NULL;  // logical and node

  log_and_node = logical_and_expr();

  if (check_t(o_or) == true) {
    match_t(o_or);
    node = malloc_expr(o_or, null, log_and_node, logical_or_expr());
  } else {
    node = log_and_node;
  }

  return node;
}

// logical_and_expr = inclusive_or_expr ( '&&' logical_and_expr )?
static struct _expr* logical_and_expr(void) {
  struct _expr* node = NULL;         // main node
  struct _expr* inc_or_node = NULL;  // inclusive or node

  inc_or_node = inclusive_or_expr();

  if (check_t(o_and) == true) {
    match_t(o_and);
    node = malloc_expr(o_and, null, inc_or_node, logical_and_expr());
  } else {
    node = inc_or_node;
  }

  return node;
}

// inclusive_or_expr = exclusive_or_expr ( '|' inclusive_or_expr )?
static struct _expr* inclusive_or_expr(void) {
  struct _expr* node = NULL;         // main node
  struct _expr* exc_or_node = NULL;  // exclusive or node

  exc_or_node = exclusive_or_expr();

  if (check_t(o_inor) == true) {
    match_t(o_inor);
    node = malloc_expr(o_inor, null, exc_or_node, inclusive_or_expr());
  } else {
    node = exc_or_node;
  }

  return node;
}

// exclusive_or_expr = and_expr ( '^' exclusive_or_expr )?
static struct _expr* exclusive_or_expr(void) {
  struct _expr* node = NULL;      // main node
  struct _expr* and_node = NULL;  // adn expression

  and_node = and_expr();

  if (check_t(o_xor) == true) {
    match_t(o_xor);
    node = malloc_expr(o_xor, null, and_node, exclusive_or_expr());
  } else {
    node = and_node;
  }

  return node;
}

// and_expr = equality_expr ( '&' and_expr )?
static struct _expr* and_expr(void) {
  struct _expr* node = NULL;      // main node
  struct _expr* equ_node = NULL;  // equality node

  equ_node = equality_expr();

  if (check_t(o_bitand) == true) {
    match_t(o_bitand);
    node = malloc_expr(o_bitand, null, equ_node, and_expr());
  } else {
    node = equ_node;
  }

  return node;
}

// equality_expr = relational_expr ( ( '==' | '!=' ) equality_expr )?
static struct _expr* equality_expr(void) {
  struct _expr* node = NULL;       // main node
  struct _expr* rela_node = NULL;  // relational node
  int code = 0;                    // current token

  rela_node = relational_expr();

  code = token->code;

  switch (code) {
    case o_eqto:   // "=="
    case o_noteq:  // "!="
      match_t(code);
      node = malloc_expr(code, null, rela_node, equality_expr());
      break;

    default:
      node = rela_node;
      break;
  }

  return node;
}

// relational_expr = shift_expr ( ( '<' | '>' | '<=' | '>=' ) relational_expr )?
static struct _expr* relational_expr(void) {
  struct _expr* node = NULL;        // main node
  struct _expr* shift_node = NULL;  // shift node
  int code = 0;                     // current token

  shift_node = shift_expr();

  code = token->code;

  switch (code) {
    case o_les:      // "<"
    case o_great:    // ">"
    case o_leseq:    // "<="
    case o_greateq:  // ">="
      match_t(code);
      node = malloc_expr(code, null, shift_node, relational_expr());
      break;

    default:
      node = shift_node;
      break;
  }

  return node;
}

// shift_expr = addictive_expr ( ( '<<' | '>>' ) shift_expr )?
static struct _expr* shift_expr(void) {
  struct _expr* node = NULL;      // main node
  struct _expr* add_node = NULL;  // addictive node
  int code = 0;                   // current token

  add_node = addictive_expr();

  code = token->code;

  switch (code) {
    case o_lshift:  // "<<"
    case o_rshift:  // ">>"
      match_t(code);
      node = malloc_expr(code, null, add_node, shift_expr());
      break;

    default:
      node = add_node;
      break;
  }

  return node;
}

// addictive_expr = multiplicative_expr ( ( '+' | '-' ) addictive_expr )?
static struct _expr* addictive_expr(void) {
  struct _expr* node = NULL;       // main node
  struct _expr* mult_node = NULL;  // multiplicative node
  int code = 0;                    // current token

  mult_node = multiplicative_expr();

  code = token->code;

  switch (code) {
    case o_plus:   // "+"
    case o_minus:  // "-"
      match_t(code);
      node = malloc_expr(code, null, mult_node, addictive_expr());
      break;

    default:
      node = mult_node;
      break;
  }

  return node;
}

// multiplicative_expr = unary_expr ( ( '*' | '/' | '%' ) multiplicative_expr )?
static struct _expr* multiplicative_expr(void) {
  struct _expr* node = NULL;        // main node
  struct _expr* unary_node = NULL;  // case node
  int code = 0;                     // current token

  unary_node = unary_expr();

  code = token->code;

  switch (code) {
    case o_multi:    // "*"
    case o_divis:    // "/"
    case o_modulus:  // "%"
      match_t(code);
      node = malloc_expr(code, null, unary_node, multiplicative_expr());
      break;

    default:
      node = unary_node;
      break;
  }

  return node;
}

// unary_expr = ( 'size' | '+' | '-' | '~' | '!' | '$' ) cast_expr
//            | postfix_expr
static struct _expr* unary_expr(void) {
  struct _expr* node = NULL;  // main node
  int unary_op = 0;           // unary operator
  int code = 0;               // current token

  code = token->code;

  switch (code) {
    case k_size:   // "size"
    case o_plus:   // "+"
    case o_minus:  // "-"
    case o_unary:  // "~"
    case o_not:    // "!"
    case s_dolar:  // "$"
      unary_op = code;
      match_t(code);
      node = malloc_expr(unary_op, null, unary_expr(), null);
      break;

    default:
      node = postfix_expr();
      break;
  }

  return node;
}

// postfix_expr = postfix_expr postfix_type | primary_expr
static struct _expr* postfix_expr(void) {
  struct _expr* node = NULL;  // main node
  int code = 0;               // current token

  code = token->code;

  switch (code) {
    case o_incr:
    case o_decr:
    case s_lprn:
      break;

    default:
      node = primary_expr();
      break;
  }

  return node;
}

// primary_expr = identifier | literal | '(' expr ')'
static struct _expr* primary_expr(void) {
  struct _expr* node = NULL;  // main node
  int code = 0;               // current token

  code = token->code;

  switch (code) {
    case t_id:
      // lookup symbol using nested-block
      {
        sym_list* s_item = lookup(token->value);
        if (s_item == null)
          fatals(current_line, "no symbol found", token->value);

        // symbols with these storage class are can be used
        // - stc_asm       -> only for function call
        // - stc_link      -> only for function call
        // - stc_var       -> for any expression
        // - stc_fn        -> only for function call

        int storage = s_item->storage;
        if (storage == stc_asm || storage == stc_link || storage == stc_var ||
            storage == stc_fn) {
        } else
          fatals(current_line,
                 "unable to use symbol in expression and it's not a valid "
                 "symbol for expression",
                 s_item->name);
      }

      node = malloc_expr(t_string, token->value, null, null);
      match_t(t_id);
      break;

    case t_int:
      // check is it a ASCII character or integer before assign to node
      if (stod(token->value) > 0 && stod(token->value) < 256)
        node = malloc_expr(t_char, token->value, null, null);
      else
        node = malloc_expr(t_int, token->value, null, null);

      match_t(t_int);
      break;

    case t_char:
      node = malloc_expr(t_char, token->value, null, null);
      match_t(t_char);
      break;

    case t_string:
      node = malloc_expr(t_string, token->value, null, null);
      match_t(t_string);
      break;

    case k_empty:
      node = malloc_expr(k_empty, token->value, null, null);
      match_t(k_empty);
      break;

    case s_lprn:
      match_t(s_lprn);
      node = expr();
      match_t(s_rprn);
      break;

    default:
      fatals(current_line, "invalid token", token_string(code));
      break;
  }

  return node;
}

/* ------- short code ------- */

// data_type = 'int' | 'char' | 'string'
static int data_type(int code) {
  switch (code) {
    case k_int:
      match_t(k_int);
      return dt_int;

    case k_char:
      match_t(k_char);
      return dt_char;

    case k_string:
      match_t(k_string);
      return dt_string;

    default:
      fatals(current_line, "need an int, char or string data type",
             token_string(code));
      return 0;
  }

  return 0;
}

// return data size in bytes according to the data type
static int ret_data_size(int code) {
  switch (code) {
    case dt_int:
      return 8;

    case dt_char:
      return 1;

    case dt_string:
      return 8;

    default:
      return 0;
  }

  return 0;
}
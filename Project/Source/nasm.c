/// linux x86_64 nasm code generator
/// @file : nasm.c
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 01:03:38
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

/* -------------------------------------------- */
/* -- I'm not an expert of assembly language -- */
/* -- not completed                          -- */
/* -------------------------------------------- */

// note:
// 1. "bss" section is stored in ram
// 2. "data" section's data doesn't change at run time
// 3 . most of the assembly codes are in "https://godbolt.org/"

#include "_main.h"

// local definitions
#define tab "\t"

/* ------- label ------- */

static int string_label = 0;  // for replace string's offset data
static int label = 0;         // for local lable(s) such as if-else

// update label's value
static void update_label(void) {
  if (label == 0)
    label += 2;
  else
    label += 5;

  // -- in one function --
  // if (label >= 200)
  //   fatald(intnull,
  //          "you cant create another if/while or another statement that used "
  //          "label number",
  //          label);
}

/* ------- data structure ------- */

// for bss section
static struct _bss_sec {
  char* body;
  struct _bss_sec* next;
} * bss_sec;

// for data section
static struct _data_sec {
  char* body;
  struct _data_sec* next;
} * data_sec;

// malloc data section structure
static struct _bss_sec* malloc_bss_sec(char* body) {
  struct _bss_sec* temp = (struct _bss_sec*)malloc(sizeof(struct _bss_sec));

  if (body == null)
    temp->body = null;
  else {
    temp->body = malloc(strlen(body) + 1);
    strcpy(temp->body, body);
  }

  temp->next = null;

  return temp;
}

// malloc data section structure
static struct _data_sec* malloc_data_sec(char* body) {
  struct _data_sec* temp = (struct _data_sec*)malloc(sizeof(struct _data_sec));

  if (body == null)
    temp->body = null;
  else {
    temp->body = malloc(strlen(body) + 1);
    strcpy(temp->body, body);
  }

  temp->next = null;

  return temp;
}

// print bss section's strings
static int print_bss(void) {
  if (bss_sec == null) return 0;
  fprintf(output_file, "\nsection .bss\n");
  struct _bss_sec* temp = bss_sec;
  while (temp != null) {
    fprintf(output_file, tab "%s\n", temp->body);
    temp = temp->next;
  }

  return 0;
}

// print data section's strings
static int print_data(void) {
  if (data_sec == null) return 0;
  fprintf(output_file, "\nsection .data\n");
  struct _data_sec* temp = data_sec;
  while (temp != null) {
    fprintf(output_file, tab "%s\n", temp->body);
    temp = temp->next;
  }

  return 0;
}

// add data to bss section
void add_bss(char* body) {
  if (bss_sec == null)
    bss_sec = malloc_bss_sec(body);
  else {
    struct _bss_sec* temp = bss_sec;
    while (temp->next != null) temp = temp->next;
    temp->next = malloc_bss_sec(body);
  }
}

// add data to data section
void add_data(char* body) {
  if (data_sec == null)
    data_sec = malloc_data_sec(body);
  else {
    struct _data_sec* temp = data_sec;
    while (temp->next != null) temp = temp->next;
    temp->next = malloc_data_sec(body);
  }
}

/* ------- register list -> parameter ------- */

// registers for function parameters
static char* reg_param[6] = {
    "rdi", "rsi", "rdx", "rcx", "r8", "r9",
};

// parameter register id
static int reg_id_for_param = -1;

// free parameter registers
int free_reg_param(void) {
  reg_id_for_param = -1;
  return reg_id_for_param;
}

// get next parameter register
static int get_reg_param(void) {
  reg_id_for_param++;

  // get current register code
  int reg_id = reg_id_for_param;

  if (reg_id_for_param >= 7) {
    free_reg_param();
    return -1;

    // if returns "-1", assembly code not emit
  }

  return reg_id;
}

/* ------- register list -> scratch ------- */

// actually I don't know wtf is this and I don't know why this code needed!
// I copied this code from an other compiler :/ so fucking sad n bad!!
// this is the first compiler I developed -_-
//
// -> update:
// -> after more than two hours of research, I know what the fuck is this :)

// register list for expression
static int reg_free[6];
static char* reg_list[6] = {"r10", "r11", "r12", "r13", "r14", "r15"};

// free all registers
void freeall_reg(void) {
  reg_free[0] = reg_free[1] = reg_free[2] = reg_free[3] = reg_free[4] =
      reg_free[5] = 1;
}

// free register
static void free_reg(int reg) {
  if (reg_free[reg] != 0) fatals(intnull, "unable to free register", null);
  reg_free[reg] = 1;
}

// allocate register
static int allocate_reg(void) {
  for (int i = 0; i < 6; i++) {
    if (reg_free[i]) {
      reg_free[i] = 0;
      return i;
    }
  }

  fatals(intnull, "out of registers", null);
  return 0;
}

/* ------- code generation ------- */

// assembly begin
void asm_file_begin(void) {
  fprintf(
      output_file,
      "; generated assembly file [%s & %s]\n; link using \"ld object_file.o - o"
      " exe_file\" command\n\n",
      __DATE__, __TIME__);

  fprintf(output_file, "%%define program _start\n\n");
  fprintf(output_file, "section .text\n" tab "global program\n");
}

// assembly end
void asm_file_end(void) {
  print_bss();
  print_data();
  fprintf(output_file,
          "\n; nano_g compiler developed by lasan nishshanka "
          "(@las_nish)\n; %s & %s",
          __DATE__, __TIME__);
}

// emit comment
void emit_comment(char* body) {
  if (body == null) return;
  fprintf(output_file, "; %s\n", body);
}

// emit inline assembly
void emit_inline_asm(char* body) {
  if (body == null) return;
  fprintf(output_file, "%s\n", body);
}

// emit function name
void emit_fn_name(char* name) {
  if (name == null) return;
  fprintf(output_file, "\n%s:\n", name);
  fprintf(output_file, tab "%-5s rbp\n", "push");
  fprintf(output_file, tab "%-5s rbp, rsp\n", "mov");
}

// emit function end
void emit_fn_end(void) {
  // check function returning type
  if (is_return == false) {
    if (is_program == true)
      fprintf(output_file, tab "%-5s rax, 0\n", "mov");
    else
      fprintf(output_file, tab "nop\n");
  }

  fprintf(output_file, tab "%-5s rbp\n", "pop");
  fprintf(output_file, tab "syscall\n");
}

// emit variable
void emit_var(int data_type, int offset, char* value) {
  char* d_type = null;

  if (data_type == dt_int) {
    d_type = malloc(strlen("QWORD") + 1);
    strcpy(d_type, "QWORD");

    fprintf(output_file, tab "%-5s %s [rbp-%d], %s\n", "mov", d_type, offset,
            value);
  } else if (data_type == dt_string) {
    d_type = malloc(strlen("QWORD") + 1);
    strcpy(d_type, "QWORD");

    // update string label
    string_label++;

    // add string to the data section
    // we used DB (Define Byte) because of one-char-per-byte
    char emit_data[strlen(value) + 20];
    snprintf(emit_data, sizeof(emit_data), "LBSTR.%d: DB %s0", string_label,
             value);
    add_data(emit_data);

    // local lable for string is "LBSTR." ( Label String )
    fprintf(output_file, tab "%-5s %s [rbp-%d], LBSTR.%d\n", "mov", d_type,
            offset, string_label);
  } else if (data_type == dt_char) {
    d_type = malloc(strlen("BYTE") + 1);
    strcpy(d_type, "BYTE");

    fprintf(output_file, tab "%-5s %s [rbp-%d], %s\n", "mov", d_type, offset,
            value);
  }
}

// emit variable in data section
void emit_var_data(int data_type, char* name, char* value) {
  char* d_type = null;

  if (data_type == dt_int || data_type == dt_string) {
    d_type = malloc(strlen("DQ") + 1);
    strcpy(d_type, "DQ");
  } else if (data_type == dt_char) {
    d_type = malloc(strlen("DB") + 1);
    strcpy(d_type, "DB");
  }

  char emit_data[strlen(name) + strlen(value) + 10];
  snprintf(emit_data, sizeof(emit_data), "%s: %s %s", name, d_type, value);

  // add data to the data section
  add_data(emit_data);
}

// emit variable in parameter
void emit_var_para(int data_type, int offset, char* name) {
  char* d_type = null;

  if (data_type == dt_int) {
    d_type = malloc(strlen("QWORD") + 1);
    strcpy(d_type, "QWORD");
  } else if (data_type == dt_string) {
    d_type = malloc(strlen("QWORD") + 1);
    strcpy(d_type, "QWORD");
  } else if (data_type == dt_char) {
    d_type = malloc(strlen("BYTE") + 1);
    strcpy(d_type, "BYTE");
  }

  // get register
  int reg_id = get_reg_param();
  // if returns "-1", assembly code not emit
  if (reg_id != -1) {
    fprintf(output_file, tab "%-5s %s [rbp-%d], %s\n", "mov", d_type, offset,
            reg_param[reg_id_for_param]);
  }
}

// emit place
void emit_place(char* name, char* fn_name) {
  fprintf(output_file, "%s.%s:\n", fn_name, name);
}

// emit goto
void emit_goto(char* name, char* fn_name) {
  fprintf(output_file, tab "%-5s %s.%s\n", "jmp", fn_name, name);
}

// emit if -> if part label
int emit_if(void) {
  update_label();
  return label;
}

// emit if -> else part label
int emit_if_else(int val) {
  fprintf(output_file, tab "%-5s .L%d\n", "jmp", val);
  fprintf(output_file, ".L%d:\n", val - 1);
  label++;

  return label;
}

// emit if-else end label
int emit_end_if_else(int val) {
  fprintf(output_file, ".L%d:\n", val - 1);
  label++;

  return label;
}

// emit while body label
int emit_while(void) {
  update_label();
  fprintf(output_file, tab "%-5s .L%d\n", "jmp", label);
  fprintf(output_file, ".L%d:\n", label + 1);

  // update break label code
  break_code = label;

  return label;
}

// emit while end label
int emit_while_end(int val) {
  fprintf(output_file, ".L%d:\n", val);
  label++;
  // fprintf(output_file, tab "%-5s .L%d\n", "jmp", val + 1);

  return label;
}

// emit break
void emit_break(void) {
  fprintf(output_file, tab "%-5s .L%d\n", "jmp", break_code);
}

// emit return
void emit_return(int reg) {
  fprintf(output_file, tab "%-5s rax, %s\n", "mov", reg_list[reg]);
  free_reg(reg);
}

/* ------- generate and emit expression ------- */

// malloc expression node
struct _expr* malloc_expr(int op, char* val, struct _expr* left,
                          struct _expr* right) {
  struct _expr* temp = (struct _expr*)malloc(sizeof(struct _expr));
  if (temp == null)
    fatals(current_line, "unable to malloc expression structure", null);

  temp->op = op;
  if (val == null)
    temp->value = null;
  else {
    temp->value = malloc(strlen(val) + 1);
    strcpy(temp->value, val);
  }

  temp->left = left;
  temp->right = right;

  return temp;
}

// generate expression
int gen_expr(struct _expr* temp) {
  int left_reg = 0;
  int right_reg = 0;

  if (temp == null) return 0;

  // get the left and right sub tree values
  if (temp->left) left_reg = gen_expr(temp->right);
  if (temp->right) right_reg = gen_expr(temp->left);

  switch (temp->op) {
    case t_id: {
      break;
    }

    case t_int: {
      // load integer value to new register and return it's number
      int r = allocate_reg();
      fprintf(output_file, tab "%-5s %s, %s\n", "mov", reg_list[r],
              temp->value);
      return r;
    }

    case t_char: {
      // load character value to new register and return it's number
      int r = allocate_reg();
      fprintf(output_file, tab "%-5s %s, %s\n", "mov", reg_list[r],
              temp->value);
      return r;
    }

    case t_string: {
      break;
    }

      // +
    case o_plus: {
      // plus two registers together and return the number of register that
      // stored value and free first register's number
      fprintf(output_file, tab "%-5s %s, %s\n", "add", reg_list[right_reg],
              reg_list[left_reg]);
      free_reg(left_reg);
      return right_reg;
    }

      // -
    case o_minus: {
      // minus second register from first register and return the number of
      // register with result and free other register
      fprintf(output_file, tab "%-5s %s, %s\n", "sub", reg_list[left_reg],
              reg_list[right_reg]);
      free_reg(right_reg);
      return left_reg;
    }

      // *
    case o_multi: {
      // multiply two registers together and returns the
      // number with result and free other register
      fprintf(output_file, tab "%-5s %s, %s\n", "imul", reg_list[right_reg],
              reg_list[left_reg]);
      free_reg(left_reg);
      return right_reg;
    }

      // /
    case o_divis: {
      // devide first register by second and returns the number of register with
      // the result and free other result
      fprintf(output_file, tab "%-5s rax, %s\n", "mov", reg_list[right_reg]);
      fprintf(output_file, tab "%-5s \n", "cqo");
      fprintf(output_file, tab "%-5s %s\n", "idiv", reg_list[left_reg]);
      fprintf(output_file, tab "%-5s %s, rax\n", "mov", reg_list[right_reg]);
      free_reg(right_reg);
      return left_reg;
    }

      //   // %
      // case o_modulus: {
      //   break;
      // }

      //   // ++
      // case o_incr: {
      //   break;
      // }

      //   // --
      // case o_decr: {
      //   break;
      // }

      // ==
    case o_eqto: {
      if (parent == k_if) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jne", cur_label);
        freeall_reg();
        return -1;  // no register
      } else if (parent == k_while) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "je", cur_label);
        freeall_reg();
        return -1;  // no register
      } else {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);

        // note:
        // 1. al is the lowest 8 bits of rax
        // 2. most likely used al as a boolean

        fprintf(output_file, tab "%-5s al\n", "sete");
        fprintf(output_file, tab "%-5s rax, al\n", "movzx");
        free_reg(left_reg);
        return right_reg;
      }
    }

      // !=
    case o_noteq: {
      if (parent == k_if) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "je", cur_label);
        freeall_reg();
        return -1;  // no register
      } else if (parent == k_while) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jne", cur_label);
        freeall_reg();
        return -1;  // no register
      } else {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s al\n", "setne");
        fprintf(output_file, tab "%-5s rax, al\n", "movzx");
        free_reg(left_reg);
        return right_reg;
      }
    }

      // <
    case o_les: {
      if (parent == k_if) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jge", cur_label);
        freeall_reg();
        return -1;  // no register
      } else if (parent == k_while) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jl", cur_label);
        freeall_reg();
        return -1;  // no register
      } else {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s al\n", "setl");
        fprintf(output_file, tab "%-5s rax, al\n", "movzx");
        free_reg(left_reg);
        return right_reg;
      }
    }

      // >
    case o_great: {
      if (parent == k_if) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jle", cur_label);
        freeall_reg();
        return -1;  // no register
      } else if (parent == k_while) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jg", cur_label);
        freeall_reg();
        return -1;  // no register
      } else {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s al\n", "setg");
        fprintf(output_file, tab "%-5s rax, al\n", "movzx");
        free_reg(left_reg);
        return right_reg;
      }
    }

      // <=
    case o_leseq: {
      if (parent == k_if) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jg", cur_label);
        freeall_reg();
        return -1;  // no register
      } else if (parent == k_while) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jle", cur_label);
        freeall_reg();
        return -1;  // no register
      } else {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s al\n", "setle");
        fprintf(output_file, tab "%-5s rax, al\n", "movzx");
        free_reg(left_reg);
        return right_reg;
      }
    }

      // >=
    case o_greateq: {
      if (parent == k_if) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jl", cur_label);
        freeall_reg();
        return -1;  // no register
      } else if (parent == k_while) {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s .L%d\n", "jge", cur_label);
        freeall_reg();
        return -1;  // no register
      } else {
        fprintf(output_file, tab "%-5s %s, %s\n", "cmp", reg_list[left_reg],
                reg_list[right_reg]);
        fprintf(output_file, tab "%-5s al\n", "setge");
        fprintf(output_file, tab "%-5s rax, al\n", "movzx");
        free_reg(left_reg);
        return right_reg;
      }
    }

      // &&
    case o_and: {
      fprintf(output_file, tab "%-5s %s, %s\n", "and", reg_list[right_reg],
              reg_list[left_reg]);
      free_reg(left_reg);
      return right_reg;
    }

      // ||
    case o_or: {
      fprintf(output_file, tab "%-5s %s, %s\n", "or", reg_list[right_reg],
              reg_list[left_reg]);
      free_reg(left_reg);
      return right_reg;
    }

      // !
    case o_not: {
      break;
    }

      //   // &
      // case o_bitand: {
      //   break;
      // }

      //   // |
      // case o_inor: {
      //   break;
      // }

      // ^
    case o_xor: {
      fprintf(output_file, tab "%-5s %s, %s\n", "xor", reg_list[right_reg],
              reg_list[left_reg]);
      free_reg(left_reg);
      return right_reg;
    }

      //   // ~
      // case o_unary: {
      //   break;
      // }

      //   // <<
      // case o_lshift: {
      //   break;
      // }

      //   // >>
      // case o_rshift: {
      //   break;
      // }

      // =
    case o_eq: {
      fprintf(output_file, tab "%-5s %s, %s ; move\n", "mov",
              reg_list[right_reg], reg_list[left_reg]);
      free_reg(left_reg);
      return right_reg;
    }

      //   // +=
      // case o_pluseq: {
      //   break;
      // }

      //   // -=
      // case o_minuseq: {
      //   break;
      // }

      //   // *=
      // case o_multieq: {
      //   break;
      // }

      //   // /=
      // case o_diviseq: {
      //   break;
      // }

      //   // %=
      // case o_moduluseq: {
      //   break;
      // }

      //   // <<=
      // case o_lshifteq: {
      //   break;
      // }

      //   // >>=
      // case o_rshifteq: {
      //   break;
      // }

      //   // &=
      // case o_bitandeq: {
      //   break;
      // }

      //   // ^=
      // case o_xoreq: {
      //   break;
      // }

      //   // |=
      // case o_inoreq: {
      //   break;
      // }

      // case k_size: {
      //   break;
      // }

    default:
      fatals(current_line,
             "invalid operator representation in expression binary tree",
             token_string(temp->op));
      return 0;
  }

  return 0;
}
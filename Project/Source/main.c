/// main file
/// @file : main.c
/// @author : lasan nishshanka (@las_nish)
/// @date 2021-05-13 & 12:16:07
/// @copyright Copyright (c) 2020-2021 lasan nishshanka

#include "_main.h"

// main function
int main(int argc, char** argv) {
  init_global();

  compiler_path = malloc(strlen(argv[0] + 1));
  strcpy(compiler_path, argv[0]);

  if (argc <= 2) {
    printf("> developed by lasan nishshanka (@las_nish)\n\n");
    fatals(intnull, "invalid input [-r|-r-o|-t][FILE_NAME]", null);
  }

  if (argc >= 4) {
    printf("> developed by lasan nishshanka (@las_nish)\n\n");
    fatals(intnull, "invalid input [-r|-r-o|-t][FILE_NAME]", null);
  }

  input_file = fopen(argv[2], "r");
  if (input_file == null) fatals(intnull, "unable to open file", argv[2]);

  input_path = malloc(strlen(argv[2] + 1));
  strcpy(input_path, argv[2]);

  // ? start timer
  clock_t begin = clock();

  if (!strcmp(argv[1], "-t")) {
    print_lexer();
  } else if (!strcmp(argv[1], "-r") || !strcmp(argv[1], "-r-o")) {
    output_path = set_file_ext(input_path, ".asm");

    output_file = fopen(output_path, "w");
    if (output_file == null)
      fatals(intnull, "unable to write file [%s]", output_path);

    // parse and generate nasm assembly file
    asm_file_begin();
    program();
    asm_file_end();

    if (!strcmp(argv[1], "-r-o")) {
      // generate object file using nasm assembler
      char nasm_gen[(strlen(output_path) * 2) + 100];
      snprintf(nasm_gen, sizeof(nasm_gen), "nasm -f elf64 '%s' -o '%s.o'",
               output_path, output_path);
      system(nasm_gen);
    }
  } else {
    printf("> developed by lasan nishshanka (@las_nish)\n\n");
    fatals(intnull, "invalid command", argv[1]);
  }

  // ? end timer
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("> time for the process is: %fms\n", time_spent * 1000);

  // test symbol scope, symbol list and symbol table
  // test_symbol();

  return EXIT_SUCCESS;
}

/* ------- custom function ------- */

// initialize global variable
void init_global(void) {
  init_lex();
  init_symbol();
  init_parser();
}

// add extension to the given name
char* set_file_ext(char* open, char* ext) {
  char* ret_name = null;
  ret_name = malloc(strlen(open) + strlen(ext) + 1);
  strcpy(ret_name, open);
  strcat(ret_name, ext);
  return ret_name;
}

/* ------- error messaegs ------- */

void fatalc(int line, char* message, int c) {
  if (line != intnull)
    printf("%s \e[0;97m%d\e[0m | ", input_path, current_line);
  printf("\e[1;31merror:\e[0m %s", message);
  if (c != intnull) printf(" [%c]", c);
  printf("\n");
  exit(1);
}

void fatald(int line, char* message, int c) {
  if (line != intnull)
    printf("%s \e[0;97m%d\e[0m | ", input_path, current_line);
  printf("\e[1;31merror:\e[0m %s", message);
  if (c != intnull) printf(" [%d]", c);
  printf("\n");
  exit(1);
}

void fatals(int line, char* message, char* s) {
  if (line != intnull)
    printf("%s \e[0;97m%d\e[0m | ", input_path, current_line);
  printf("\e[1;31merror:\e[0m %s", message);
  if (s != null) printf(" [%s]", s);
  printf("\n");
  exit(1);
}
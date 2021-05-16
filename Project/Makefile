# makefile for nano_g compiler v1.0.0
# start -> may 12, 2020 & 10:40 p.m.
# end   -> may 16, 2020 & 04:58 p.m.

# system info:
# ubuntu 20.1 LTS
# amd64 64bit

# folder list

f_export = Export/
f_source = Source/
f_test = Test/

f_asm = $(f_test)Assembly\ Test/
f_lex = $(f_test)Lexical\ Test/

# source list

source = $(f_source)lexer.c $(f_source)main.c $(f_source)nasm.c $(f_source)parser.c $(f_source)preprocessor.c $(f_source)symbol.c

# command

disable_warn = -Wno-unused-value -Wno-unused-function
command = gcc -m64 -Wall $(disable_warn) -o

# executable name

exe = $(f_export)nano_g

# build project
BUILD:
	@$(command) $(exe) $(source)

# run project
RUN:
	@./$(exe) -r-o $(f_asm)Test\ 01/test_01.ng
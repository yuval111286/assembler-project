# Compilation macros
CC = gcc
CFLAGS = -ansi -Wall -pedantic -g
GLOBAL_DEPS = globals.h user_interface.h
EXE_DEPS = assembler.o utils.o symbol_table.o preprocessor.o preprocessor_helper.o first_pass.o first_pass_helper.o second_pass.o second_pass_helper.o linked_list.o errors_handler.o parser.o parser_helper.o code_image.o

# Executable target
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

assembler.o: assembler.c $(GLOBAL_DEPS)
	$(CC) -c assembler.c $(CFLAGS) -o $@

preprocessor.o: preprocessor.c preprocessor.h preprocessor_helper.h $(GLOBAL_DEPS)
	$(CC) -c preprocessor.c $(CFLAGS) -o $@

preprocessor_helper.o: preprocessor_helper.c preprocessor_helper.h $(GLOBAL_DEPS)
	$(CC) -c preprocessor_helper.c $(CFLAGS) -o $@

first_pass.o: first_pass.c first_pass.h first_pass_helper.h $(GLOBAL_DEPS) code_image.h
	$(CC) -c first_pass.c $(CFLAGS) -o $@

first_pass_helper.o: first_pass_helper.c first_pass_helper.h $(GLOBAL_DEPS)
	$(CC) -c first_pass_helper.c $(CFLAGS) -o $@

second_pass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(CC) -c second_pass.c $(CFLAGS) -o $@

second_pass_helper.o: second_pass_helper.c second_pass_helper.h $(GLOBAL_DEPS)
	$(CC) -c second_pass_helper.c $(CFLAGS) -o $@

linked_list.o: linked_list.c linked_list.h $(GLOBAL_DEPS)
	$(CC) -c linked_list.c $(CFLAGS) -o $@

symbol_table.o: symbol_table.c symbol_table.h $(GLOBAL_DEPS)
	$(CC) -c symbol_table.c $(CFLAGS) -o $@

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

errors_handler.o: errors_handler.c errors_handler.h $(GLOBAL_DEPS)
	$(CC) -c errors_handler.c $(CFLAGS) -o $@

parser.o: parser.c parser.h parser_helper.h $(GLOBAL_DEPS)
	$(CC) -c parser.c $(CFLAGS) -o $@

parser_helper.o: parser_helper.c parser_helper.h $(GLOBAL_DEPS)
	$(CC) -c parser_helper.c $(CFLAGS) -o $@

code_image.o: code_image.c code_image.h $(GLOBAL_DEPS)
	$(CC) -c code_image.c $(CFLAGS) -o $@

clean:
	rm -rf *.o *.am *.ob *.ent *.ext assembler
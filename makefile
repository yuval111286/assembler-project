# Compilation macros
CC = gcc
CFLAGS = -ansi -Wall -pedantic -g
GLOBAL_DEPS = globals.h
EXE_DEPS = assembler_prog.o utils.o symbol_table.o preprocessor.o first_pass.o second_pass.o linked_list.o errors_handler.o analyze_text.o parser.o code_image.o

# Executable target
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

assembler_prog.o: assembler_prog.c $(GLOBAL_DEPS)
	$(CC) -c assembler_prog.c $(CFLAGS) -o $@

preprocessor.o: preprocessor.c preprocessor.h $(GLOBAL_DEPS)
	$(CC) -c preprocessor.c $(CFLAGS) -o $@

first_pass.o: first_pass.c first_pass.h $(GLOBAL_DEPS) code_image.h
	$(CC) -c first_pass.c $(CFLAGS) -o $@

second_pass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(CC) -c second_pass.c $(CFLAGS) -o $@

linked_list.o: linked_list.c linked_list.h $(GLOBAL_DEPS)
	$(CC) -c linked_list.c $(CFLAGS) -o $@

symbol_table.o: symbol_table.c symbol_table.h $(GLOBAL_DEPS)
	$(CC) -c symbol_table.c $(CFLAGS) -o $@

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

errors_handler.o: errors_handler.c errors_handler.h $(GLOBAL_DEPS)
	$(CC) -c errors_handler.c $(CFLAGS) -o $@

analyze_text.o: analyze_text.c analyze_text.h $(GLOBAL_DEPS)
	$(CC) -c analyze_text.c $(CFLAGS) -o $@

parser.o: parser.c parser.h $(GLOBAL_DEPS)
	$(CC) -c parser.c $(CFLAGS) -o $@

code_image.o: code_image.c code_image.h $(GLOBAL_DEPS)
	$(CC) -c code_image.c $(CFLAGS) -o $@

clean:
	rm -rf *.o *.am *.ob *.ent *.ext

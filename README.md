# Multi-Pass Assembler (ANSI-C | C90)

This project is a fully functional **multi-pass assembler** implemented in strict ANSI-C (C90), built as part of a system programming course.  
It includes macro processing, symbol table management, two-pass parsing, memory handling, and generation of machine code output files.

## Features
- **Macro Processing:** Pre-assembler that expands user-defined macros.
- **Two-Pass Assembly:**
  - First pass: Symbol table creation, instruction/data counting, error detection.
  - Second pass: Resolving addresses, encoding instructions, and writing machine code.
- **Complete Output Generation:**
  - `.ob` – machine code in base-4 format  
  - `.ent` – entry labels  
  - `.ext` – external label references
- **Error Handling:** Comprehensive syntax and semantic error detection.

## Files Overview
- `assembler.c` – Main assembly flow
- `first_pass.c` / `second_pass.c` – Parsing and encoding logic
- `code_image.c` – Machine code image handling
- `errors_handler.c` – Error reporting system
- `*.as` – Example assembly input files

## How to Run
Compile using a standard ANSI-C compiler such as `gcc`:

```bash
gcc -ansi -Wall -pedantic *.c -o assembler
./assembler input.as

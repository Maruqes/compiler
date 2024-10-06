#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
\
extern size_t custom_code_size;

typedef struct
{
    uint32_t opcode_index;
    uint32_t offset_in_opcode;
    char *symbol_name;
} Fixup;

typedef struct
{
    size_t size; // Number of bytes in the opcode
    char *code;  // Pointer to the opcode bytes
} OpCode;

extern Fixup *fixups_array;
extern uint32_t fixups_array_size;

extern OpCode *op_codes_array;
extern uint32_t op_codes_array_size;

void mov_eax(uint32_t code);
void mov_ebx(uint32_t code);
void mov_ecx(uint32_t code);
void mov_edx(uint32_t code);
void mov_edi(uint32_t code);
void our_syscall();

void mov_var_from_eax(uint32_t code);

void add_custom_code_size();
void write_all_custom_code(int __fd);

void mov_ecx_symbol(char *symbol_name);

void cleanup();
#endif // FUNCTIONS_H
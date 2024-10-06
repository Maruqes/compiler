#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

extern size_t custom_code_size;

typedef struct
{
    uint32_t opcode_index;
    uint32_t offset_in_opcode;
    char *symbol_name;
    uint32_t var_offset;
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

// mov eax, val
void mov_eax(uint32_t code);
void mov_ebx(uint32_t code);
void mov_ecx(uint32_t code);
void mov_edx(uint32_t code);
void mov_esi(uint32_t code);
void mov_edi(uint32_t code);
void our_syscall();

void add_custom_code_size();
void write_all_custom_code(int __fd);

// mov eax, var
void mov_eax_symbol_address(char *symbol_name);
void mov_ebx_symbol_address(char *symbol_name);
void mov_ecx_symbol_address(char *symbol_name);
void mov_edx_symbol_address(char *symbol_name);
void mov_esi_symbol_address(char *symbol_name);
void mov_edi_symbol_address(char *symbol_name);

// mov var, reg
void mov_var_from_eax(char *symbol, int var_offset);
void mov_var_from_ebx(char *symbol, int var_offset);
void mov_var_from_ecx(char *symbol, int var_offset);
void mov_var_from_edx(char *symbol, int var_offset);
void mov_var_from_esi(char *symbol, int var_offset);
void mov_var_from_edi(char *symbol, int var_offset);

void cleanup();

void add_fixup(int index, char *symbol_name, int offset, uint32_t var_offset);

#endif // FUNCTIONS_H
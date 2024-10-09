#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define REG_EAX 0x0
#define REG_EBX 0x3
#define REG_ECX 0x1
#define REG_EDX 0x2
#define REG_ESI 0x6
#define REG_EDI 0x7

extern size_t custom_code_size;

typedef struct
{
    uint32_t opcode_index;
    uint32_t offset_in_opcode; // Offset where the address should be inserted
    char *symbol_name;
    uint32_t var_offset;  // offset of the variable in the data section
    uint32_t code_offset; // offset of the instruction in the code
} Fixup;

typedef struct
{
    size_t size; // Number of bytes in the opcode
    char *code;  // Pointer to the opcode bytes
} OpCode;

typedef struct Jump_struct
{
    char *var_name;
    uint32_t var_address;
    uint8_t small_jump;
    uint32_t fix_addr;
} Jump_struct;

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
void mov_eax_symbol_address(char *symbol_name, int var_offset);
void mov_ebx_symbol_address(char *symbol_name, int var_offset);
void mov_ecx_symbol_address(char *symbol_name, int var_offset);
void mov_edx_symbol_address(char *symbol_name, int var_offset);
void mov_esi_symbol_address(char *symbol_name, int var_offset);
void mov_edi_symbol_address(char *symbol_name, int var_offset);

// mov var, reg
void mov_var_from_eax(char *symbol, int var_offset);
void mov_var_from_ebx(char *symbol, int var_offset);
void mov_var_from_ecx(char *symbol, int var_offset);
void mov_var_from_edx(char *symbol, int var_offset);
void mov_var_from_esi(char *symbol, int var_offset);
void mov_var_from_edi(char *symbol, int var_offset);

// mov reg, var
void mov_eax_from_var(char *symbol, int var_offset);
void mov_ebx_from_var(char *symbol, int var_offset);
void mov_ecx_from_var(char *symbol, int var_offset);
void mov_edx_from_var(char *symbol, int var_offset);
void mov_esi_from_var(char *symbol, int var_offset);
void mov_edi_from_var(char *symbol, int var_offset);

void inc_eax();
void inc_ebx();
void inc_ecx();
void inc_edx();
void inc_esi();
void inc_edi();

void cleanup();

void add_fixup(int index, char *symbol_name, int offset, uint32_t var_offset);

void jmp_reg32(uint8_t reg_code);

void create_label(char *name, uint8_t small_jump);
void fix_label_addresses(uint32_t fix_size);
void jmp(char *name);
void small_jump(char *name);

void cmp_reg32(uint8_t reg1, uint8_t reg2);

#endif // FUNCTIONS_H
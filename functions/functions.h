#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define REG_EAX 0x0
#define REG_ECX 0x1
#define REG_EDX 0x2
#define REG_EBX 0x3
#define REG_ESP 0x4
#define REG_EBP 0x5
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
    uint32_t jump_offset; // if a jump (good type, not the eax jump) this should be the lenght of the bytes of the jump
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
void mov_reg32(uint8_t reg_code, uint32_t value);
void our_syscall();

void add_custom_code_size();
void write_all_custom_code(int __fd);

// ALL OF VARS HERE ARE REALLY CONSTANTS IN THE DATA SECTION (that can be actually changed using mov_var_from_reg32)

// mov reg, var        //the actual address of the variable
void mov_reg32_symbol_address(uint8_t reg_code, char *symbol_name, int var_offset);
void mov_eax_symbol_address(char *symbol_name, int var_offset);
void mov_ebx_symbol_address(char *symbol_name, int var_offset);
void mov_ecx_symbol_address(char *symbol_name, int var_offset);
void mov_edx_symbol_address(char *symbol_name, int var_offset);
void mov_esi_symbol_address(char *symbol_name, int var_offset);
void mov_edi_symbol_address(char *symbol_name, int var_offset);

// mov [var + var_offset], reg
void mov_var_from_reg32(uint8_t reg_code, char *symbol, int var_offset);
void mov_var_from_eax(char *symbol, int var_offset);
void mov_var_from_ebx(char *symbol, int var_offset);
void mov_var_from_ecx(char *symbol, int var_offset);
void mov_var_from_edx(char *symbol, int var_offset);
void mov_var_from_esi(char *symbol, int var_offset);
void mov_var_from_edi(char *symbol, int var_offset);

// mov reg, [var + var_offset]
void mov_reg32_from_var(uint8_t reg_code, char *symbol, int var_offset);
void mov_eax_from_var(char *symbol, int var_offset);
void mov_ebx_from_var(char *symbol, int var_offset);
void mov_ecx_from_var(char *symbol, int var_offset);
void mov_edx_from_var(char *symbol, int var_offset);
void mov_esi_from_var(char *symbol, int var_offset);
void mov_edi_from_var(char *symbol, int var_offset);

// extra
void mov_reg_reg_offset(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void mov_reg_with_regOffset_value(uint8_t reg, uint8_t reg2, uint32_t value);
void mov_reg_reg_with_offset(uint8_t reg, uint8_t reg_base, uint8_t reg_offset);
void mov_reg_with_regOffset_reg(uint8_t reg_base, uint8_t reg2, uint8_t reg3);
void mov_reg_offset_value(uint8_t reg, int32_t offset, uint32_t value);
void mov_reg_offset_reg2(uint8_t reg, uint32_t offset, uint8_t reg2);
void mov_var_from_reg32_offREG(uint8_t reg_code, char *symbol, uint8_t reg2);
void mov_reg32_from_var_offREG(uint8_t reg_code, char *symbol, uint8_t reg2);

void inc_eax();
void inc_ebx();
void inc_ecx();
void inc_edx();
void inc_esi();
void inc_edi();

void cleanup();

void add_fixup(int index, char *symbol_name, int offset, uint32_t var_offset, uint32_t jump_offset);

void cmp_reg32(uint8_t reg1, uint8_t reg2);

void create_label(char *name);
void fix_label_addresses(uint32_t fix_size);
void ret();
void nop();

#endif // FUNCTIONS_H
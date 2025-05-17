#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MOD_NO_DISP 0b00 << 6    // 0b00000000
#define MOD_1BYTE_DISP 0b01 << 6 // 0b01000000
#define MOD_4BYTE_DISP 0b10 << 6 // 0b10000000
#define MOD_REG_DIRECT 0b11 << 6 // 0b11000000

// Macros para extrair os bits corretos
#define REG_FIELD(x) ((x & 0b111) << 3) // bits 3 a 5 0b111000
#define RM_FIELD(x) (x & 0b111)         // bits 0 a 2 0b111

#define RM_SIB 0b100
#define SCALE_1 (0b00 << 6)
#define SCALE_2 (0b01 << 6)
#define SCALE_4 (0b10 << 6)
#define SCALE_8 (0b11 << 6)

#define INDEX_FIELD(x) ((x & 0b111) << 3) /* bits 5-3 */
#define BASE_FIELD(x) (x & 0b111)         /* bits 2-0  */

extern size_t custom_code_size;
extern size_t data_size;

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

extern Jump_struct *jump_array;
extern uint32_t jump_array_size;

// mov eax, val
void mov_eax(uint32_t code);
void mov_ebx(uint32_t code);
void mov_ecx(uint32_t code);
void mov_edx(uint32_t code);
void mov_esi(uint32_t code);
void mov_edi(uint32_t code);
void mov_reg32(uint8_t reg_code, uint32_t value);
void our_syscall();

uint64_t add_custom_code_size();
void write_all_custom_code(int __fd);

void free_fixups();

void fixup_addresses();

void cleanup();

void add_fixup(int index, char *symbol_name, int offset, uint32_t var_offset, uint32_t jump_offset);

void nop();

void set_modrm(uint8_t *dst, uint8_t mod, uint8_t reg, uint8_t rm);
void set_sib(uint8_t *dst, uint8_t scale, uint8_t index, uint8_t base);
int precisa_sib(uint8_t mod, uint8_t reg_base, int usa_index);

#endif // FUNCTIONS_H
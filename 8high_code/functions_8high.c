// functions_16.c

#include "functions_8high.h"
#include "../functions.h" // Adjust the path as needed
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define REG_AH 0x4
#define REG_CH 0x5
#define REG_DH 0x6
#define REG_BH 0x7

void mov_reg8_high(uint8_t reg_code, uint8_t value)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xB0 + reg_code; // Opcode for 'mov reg8, imm8'

    opcode_bytes[1] = value; // Immediate value

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Functions to move immediate values into high 8-bit registers
void mov_ah(uint8_t value) { mov_reg8_high(REG_AH, value); }
void mov_ch(uint8_t value) { mov_reg8_high(REG_CH, value); }
void mov_dh(uint8_t value) { mov_reg8_high(REG_DH, value); }
void mov_bh(uint8_t value) { mov_reg8_high(REG_BH, value); }

void mov_reg8_high_from_var(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x8A;                   // Opcode for 'mov r8, r/m8'
    opcode_bytes[1] = (reg_code << 3) | 0x05; // ModR/M byte

    memset(&opcode_bytes[2], 0, 4); // Placeholder for the address

    OpCode new_opcode;
    new_opcode.size = 6;
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 2, var_offset);

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Helper functions for high 8-bit registers
void mov_ah_from_var(char *symbol, int var_offset) { mov_reg8_high_from_var(REG_AH, symbol, var_offset); }
void mov_ch_from_var(char *symbol, int var_offset) { mov_reg8_high_from_var(REG_CH, symbol, var_offset); }
void mov_dh_from_var(char *symbol, int var_offset) { mov_reg8_high_from_var(REG_DH, symbol, var_offset); }
void mov_bh_from_var(char *symbol, int var_offset) { mov_reg8_high_from_var(REG_BH, symbol, var_offset); }

void mov_var_from_reg8_high(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x88;                   // Opcode for 'mov r/m8, reg8'
    opcode_bytes[1] = (reg_code << 3) | 0x05; // ModR/M byte

    memset(&opcode_bytes[2], 0, 4); // Placeholder for the address

    OpCode new_opcode;
    new_opcode.size = 6;
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 2, var_offset);

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Helper functions for high 8-bit registers
void mov_var_from_ah(char *symbol, int var_offset) { mov_var_from_reg8_high(REG_AH, symbol, var_offset); }
void mov_var_from_ch(char *symbol, int var_offset) { mov_var_from_reg8_high(REG_CH, symbol, var_offset); }
void mov_var_from_dh(char *symbol, int var_offset) { mov_var_from_reg8_high(REG_DH, symbol, var_offset); }
void mov_var_from_bh(char *symbol, int var_offset) { mov_var_from_reg8_high(REG_BH, symbol, var_offset); }

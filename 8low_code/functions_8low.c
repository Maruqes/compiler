// functions_16.c

#include "functions_8low.h"
#include "../functions.h" // Adjust the path as needed
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void mov_reg8(uint8_t reg_code, uint8_t value)
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

// Functions to move immediate values into 8-bit registers
void mov_al(uint8_t value) { mov_reg8(REG_AL, value); }
void mov_cl(uint8_t value) { mov_reg8(REG_CL, value); }
void mov_dl(uint8_t value) { mov_reg8(REG_DL, value); }
void mov_bl(uint8_t value) { mov_reg8(REG_BL, value); }

void mov_reg8_from_var(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(6); // 1-byte opcode + 1-byte ModR/M + 4-byte address
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x8A;                   // Opcode for 'mov r8, r/m8'
    opcode_bytes[1] = (reg_code << 3) | 0x05; // ModR/M byte

    memset(&opcode_bytes[2], 0, 4); // Placeholder for the 4-byte address

    OpCode new_opcode;
    new_opcode.size = 6; // 1-byte opcode + 1-byte ModR/M + 4-byte address
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 2, var_offset, 0);

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Helper functions for low 8-bit registers
void mov_al_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_AL, symbol, var_offset); }
void mov_cl_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_CL, symbol, var_offset); }
void mov_dl_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_DL, symbol, var_offset); }
void mov_bl_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_BL, symbol, var_offset); }

void mov_var_from_reg8(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(6); // 1-byte opcode + 1-byte ModR/M + 4-byte address
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x88;                   // Opcode for 'mov r/m8, r8'
    opcode_bytes[1] = (reg_code << 3) | 0x05; // ModR/M byte

    memset(&opcode_bytes[2], 0, 4); // Placeholder for the 4-byte address

    OpCode new_opcode;
    new_opcode.size = 6; // 1-byte opcode + 1-byte ModR/M + 4-byte address
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 2, var_offset, 0);

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Helper functions for low 8-bit registers
void mov_var_from_al(char *symbol, int var_offset) { mov_var_from_reg8(REG_AL, symbol, var_offset); }
void mov_var_from_cl(char *symbol, int var_offset) { mov_var_from_reg8(REG_CL, symbol, var_offset); }
void mov_var_from_dl(char *symbol, int var_offset) { mov_var_from_reg8(REG_DL, symbol, var_offset); }
void mov_var_from_bl(char *symbol, int var_offset) { mov_var_from_reg8(REG_BL, symbol, var_offset); }

void cmp_reg8(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x38;
    opcode_bytes[1] = 0xC0 + reg1 + (reg2 * 8);

    OpCode new_opcode;
    new_opcode.size = 2; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}
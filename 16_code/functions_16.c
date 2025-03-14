// functions_16.c

#include "functions_16.h"
#include "../functions/functions.h" // Adjust the path as needed
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Define register codes for 16-bit registers

// Function to move immediate value into a 16-bit register
void mov_reg16(uint8_t reg_code, uint16_t value)
{
    char *opcode_bytes = malloc(4); // 1-byte prefix + 1-byte opcode + 2-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;            // Operand-size override prefix
    opcode_bytes[1] = 0xB8 + reg_code; // Opcode for 'mov reg16, imm16'

    memcpy(&opcode_bytes[2], &value, sizeof(value)); // Copy the 2-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 4;
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

// Functions to move immediate values into 16-bit registers
void mov_ax(uint16_t value) { mov_reg16(REG_AX, value); }
void mov_bx(uint16_t value) { mov_reg16(REG_BX, value); }
void mov_cx(uint16_t value) { mov_reg16(REG_CX, value); }
void mov_dx(uint16_t value) { mov_reg16(REG_DX, value); }
void mov_si(uint16_t value) { mov_reg16(REG_SI, value); }
void mov_di(uint16_t value) { mov_reg16(REG_DI, value); }

// Function to move data from memory to a 16-bit register
void mov_reg16_from_var(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(7); // Corrected to 7 bytes
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // Operand-size override prefix
    opcode_bytes[1] = 0x8B; // Opcode for 'mov reg16, r/m16'
    opcode_bytes[2] = 0x05; // ModR/M byte for 'disp32' addressing mode

    memset(&opcode_bytes[3], 0, 4); // Placeholder for address

    // Modify the ModR/M byte to set the correct register code
    opcode_bytes[2] = (reg_code << 3) | 0x05;

    OpCode new_opcode;
    new_opcode.size = 7; // 1-byte prefix + 1-byte opcode + 1-byte ModR/M + 4-byte address
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 3, var_offset, 0); // Offset is 3 due to prefix, opcode, and ModR/M

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

// Functions to move data from memory to 16-bit registers
void mov_ax_from_var(char *symbol, int var_offset) { mov_reg16_from_var(REG_AX, symbol, var_offset); }
void mov_bx_from_var(char *symbol, int var_offset) { mov_reg16_from_var(REG_BX, symbol, var_offset); }
void mov_cx_from_var(char *symbol, int var_offset) { mov_reg16_from_var(REG_CX, symbol, var_offset); }
void mov_dx_from_var(char *symbol, int var_offset) { mov_reg16_from_var(REG_DX, symbol, var_offset); }
void mov_si_from_var(char *symbol, int var_offset) { mov_reg16_from_var(REG_SI, symbol, var_offset); }
void mov_di_from_var(char *symbol, int var_offset) { mov_reg16_from_var(REG_DI, symbol, var_offset); }

// Function to move data from a 16-bit register to memory
void mov_var_from_reg16(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(7); // Corrected to 7 bytes
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // Operand-size override prefix
    opcode_bytes[1] = 0x89; // Opcode for 'mov r/m16, reg16'
    opcode_bytes[2] = 0x05; // ModR/M byte for 'disp32' addressing mode

    memset(&opcode_bytes[3], 0, 4); // Placeholder for address

    // Modify the ModR/M byte to set the correct register code
    opcode_bytes[2] = (reg_code << 3) | 0x05;

    OpCode new_opcode;
    new_opcode.size = 7; // 1-byte prefix + 1-byte opcode + 1-byte ModR/M + 4-byte address
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 3, var_offset, 0); // Offset is 3 due to prefix, opcode, and ModR/M

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

// Functions to move data from 16-bit registers to memory
void mov_var_from_ax(char *symbol, int var_offset) { mov_var_from_reg16(REG_AX, symbol, var_offset); }
void mov_var_from_bx(char *symbol, int var_offset) { mov_var_from_reg16(REG_BX, symbol, var_offset); }
void mov_var_from_cx(char *symbol, int var_offset) { mov_var_from_reg16(REG_CX, symbol, var_offset); }
void mov_var_from_dx(char *symbol, int var_offset) { mov_var_from_reg16(REG_DX, symbol, var_offset); }
void mov_var_from_si(char *symbol, int var_offset) { mov_var_from_reg16(REG_SI, symbol, var_offset); }
void mov_var_from_di(char *symbol, int var_offset) { mov_var_from_reg16(REG_DI, symbol, var_offset); }

void push_reg16(uint8_t reg_code)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x50 + reg_code;

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void push_ax() { push_reg16(REG_AX); }
void push_bx() { push_reg16(REG_BX); }
void push_cx() { push_reg16(REG_CX); }
void push_dx() { push_reg16(REG_DX); }
void push_si() { push_reg16(REG_SI); }
void push_di() { push_reg16(REG_DI); }

void pop_reg16(uint8_t reg_code)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x58 + reg_code;

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void pop_ax() { pop_reg16(REG_AX); }
void pop_bx() { pop_reg16(REG_BX); }
void pop_cx() { pop_reg16(REG_CX); }
void pop_dx() { pop_reg16(REG_DX); }
void pop_si() { pop_reg16(REG_SI); }
void pop_di() { pop_reg16(REG_DI); }

void cmp_reg16(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x39;
    opcode_bytes[2] = 0xC0 + reg1 + (reg2 * 8);

    OpCode new_opcode;
    new_opcode.size = 3; // Total instruction size
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
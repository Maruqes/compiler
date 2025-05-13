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
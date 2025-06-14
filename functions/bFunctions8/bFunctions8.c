#include "bFunctions8.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
/*
i-immediate
r-register
m-memory

functions:

mov8 r, i  :D
mov8 r, m  :D
mov8 r, mi :D
mov8 r, mr :D
mov8 r, r  :D

mov8 m, i  :D
mov8 m, r  :D

mov8 mi, i :D
mov8 mi, r :D

mov8 mr, i
mov8 mr, r
*/

void mov8_r_i(uint8_t reg_code, uint8_t value)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xb0 + (reg_code);
    opcode_bytes[1] = value; // Immediate value

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes); // Free the just allocated memory to prevent leaks
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov8_r_m(uint8_t reg, uint8_t mem_reg)
{
}

void mov8_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
}

void mov8_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
}

void mov8_r_r(uint8_t reg1, uint8_t reg2)
{
}

void mov8_m_i(uint8_t reg1, uint8_t value)
{
}

void mov8_m_r(uint8_t reg1, uint8_t reg2)
{
}

void mov8_mi_i(uint8_t reg, int32_t offset, uint32_t value)
{
}

void mov8_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
}

void mov8_mr_i(uint8_t reg, uint8_t reg2, uint32_t value)
{
}

void mov8_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
}

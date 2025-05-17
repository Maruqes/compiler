#include "bFunctions32.h"
#include "../functions.h"

/*
i-immediate
r-register
m-memory

functions:

mov32 r, i  :D
mov32 r, m  :D
mov32 r, mi :D
mov32 r, mr :D
mov32 r, r  :D

mov32 m, i  :D
mov32 m, r  :D

mov32 mi, i :D
mov32 mi, r :D

mov32 mr, i
mov32 mr, r
*/

void set_67_prefix(char *opcode_bytes)
{
    opcode_bytes[0] = 0x67; // Prefix for 32-bit operand size
}

// Function to move immediate value into a 32-bit register
void mov32_r_i(uint8_t reg_code, uint32_t value)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xB8 + (reg_code); // Opcode for 'mov r64, imm64'
    memcpy(&opcode_bytes[1], &value, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov32_r_m(uint8_t reg, uint8_t mem_reg)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, mem_reg, 0);

    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_67_prefix(opcode_bytes); // Set the 66 prefix for 32-bit operand size
    opcode_bytes[1] = 0x8B;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg, mem_reg);
    set_sib(&opcode_bytes[3], 0, RM_SIB, mem_reg);
    opcode_bytes[3 + sib_needed] = 0x00; // Displacement byte (not used in this case)

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
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

void mov32_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
}

void mov32_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
}

void mov32_r_r(uint8_t reg1, uint8_t reg2)
{
}

void mov32_m_i(uint8_t reg1, uint32_t value)
{
}

void mov32_m_r(uint8_t reg1, uint8_t reg2)
{
}

void mov32_mi_i(uint8_t reg, int32_t offset, uint32_t value)
{
}

// works for all :D
void mov32_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
}

void mov32_mr_i(uint8_t reg, uint8_t reg2, uint32_t value)
{
}

void mov32_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
}

void sete_r(uint8_t reg)
{
}

void setne_r(uint8_t reg)
{
}

void setl_r(uint8_t reg)
{
}

void setg_r(uint8_t reg)
{
}

void shl_reg_imm(uint8_t reg, uint8_t imm)
{
}

void shr_reg_imm(uint8_t reg, uint8_t imm)
{
}

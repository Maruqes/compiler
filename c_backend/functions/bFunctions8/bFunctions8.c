#include "bFunctions8.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
#include "../bFunctions32/bFunctions32.h"
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
    uint8_t rm = reg_code; // lower 3 bits select the register in ModRM

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg_code >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC6;                             // MOV r/m8, imm8 (opcode C6 /0)
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 0, rm); // /0, Mod=11, r/m=register
    opcode_bytes[3] = (char)value;                      // imm8

    OpCode new_opcode;
    new_opcode.size = 4;
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
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, mem_reg, 0);

    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg >= REG_R8) ? 1 : 0, 0, (mem_reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8A;
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

void mov8_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg_base, 0);

    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg_dest >= REG_R8) ? 1 : 0, 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8A;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg_dest, reg_base);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg_base);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(int32_t)); // Copy the offset

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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

void mov8_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
    // In SIB, index cannot be RSP (code 4) regardless of extension.
    if ((reg_offset & 0x7) == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg_base, 1);

    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // REX: R extends 'reg' (ModRM.reg), X extends index (reg_offset), B extends base
    set_rex_prefix(opcode_bytes,
                   0,
                   (reg >= REG_R8) ? 1 : 0,
                   (reg_offset >= REG_R8) ? 1 : 0,
                   (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8A;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg & 0x7, RM_SIB);
    set_sib(&opcode_bytes[3], 0, reg_offset & 0x7, reg_base & 0x7);
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

void mov8_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // REX for byte regs; R extends reg2 (source), B extends reg1 (dest)
    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x88;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2, reg1);

    OpCode new_opcode;
    new_opcode.size = 3;
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

void mov8_m_i(uint8_t reg1, uint8_t value)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(5 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC6;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, reg1);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg1);

    opcode_bytes[3 + sib_needed] = 0x00;                            // Displacement byte (not used in this case)
    memcpy(&opcode_bytes[4 + sib_needed], &value, sizeof(uint8_t)); // Copy the immediate value

    OpCode new_opcode;
    new_opcode.size = 5 + sib_needed;
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

void mov8_m_r(uint8_t reg1, uint8_t reg2)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x88;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg2, reg1);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg1);
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

void mov8_mi_i(uint8_t reg, int32_t offset, uint8_t value)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(8 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC6;

    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 0, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(int32_t)); // Copy the offset
    memcpy(&opcode_bytes[7 + sib_needed], &value, sizeof(uint8_t));  // Copy the immediate value

    OpCode new_opcode;
    new_opcode.size = 8 + sib_needed;
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

void mov8_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x88;

    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg2, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(int32_t)); // Copy the offset

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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

void mov8_mr_i(uint8_t reg, uint8_t reg2, uint8_t value)
{
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = 1;

    char *opcode_bytes = malloc(5 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, (reg2 >= REG_R8) ? 1 : 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC6;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg2, reg);
    opcode_bytes[4] = 0x00;                            // Displacement byte (not used in this case)
    memcpy(&opcode_bytes[5], &value, sizeof(uint8_t)); // Copy the immediate value

    OpCode new_opcode;
    new_opcode.size = 5 + sib_needed;
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

void mov8_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = 1;

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x88;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg3, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg2, reg_base);
    opcode_bytes[4] = 0x00; // Displacement byte (not used in this case)

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

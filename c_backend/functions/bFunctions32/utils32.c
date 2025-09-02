#include "bFunctions32.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"

// Now a no-op: extended registers are supported via REX in 32-bit ops too
void cant_use_rx(uint8_t reg[], size_t size)
{
    (void)reg;
    (void)size;
}

void cmp32_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg2 >= 8) ? 1 : 0, 0, (reg1 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x39;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2 & 0x7, reg1 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void cmp32_r_i(uint8_t reg1, uint32_t imm32)
{
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg1 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x81;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 7, reg1 & 0x7);
    memcpy(&opcode_bytes[3], &imm32, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void cmp32_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x3B;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void cmp32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x3B;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void lshfit32(uint8_t reg, uint8_t imm)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xC1;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg & 0x7);
    opcode_bytes[3] = imm;

    OpCode new_opcode;
    new_opcode.size = 4;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void rshfit32(uint8_t reg, uint8_t imm)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xC1;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg & 0x7);
    opcode_bytes[3] = imm;

    OpCode new_opcode;
    new_opcode.size = 4;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void lshfit32_reg(uint8_t reg1)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg1 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xD3;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg1 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void rshfit32_reg(uint8_t reg1)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg1 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xD3;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg1 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Logical operations: AND, OR, XOR, NOT

void and32_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg2 >= 8) ? 1 : 0, 0, (reg1 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x21;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2 & 0x7, reg1 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and32_r_i(uint8_t reg, uint32_t imm32)
{
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x81;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg & 0x7);
    memcpy(&opcode_bytes[3], &imm32, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and32_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x23;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed + sizeof(uint32_t));
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x23;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed + sizeof(uint32_t);
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

    if (reg3 == REG_ESP) // ESP RSP ALL SAME SHIT ITS ALL "4"
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 1);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0,
                   (reg1 >= 8) ? 1 : 0,
                   (reg3 >= 8) ? 1 : 0,
                   (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x23;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, RM_SIB);
    set_sib(&opcode_bytes[3], 0, reg3 & 0x7, reg2 & 0x7);
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// OR operations: OR r,r r,m r,i r,mr

void or32_r_r(uint8_t reg1, uint8_t reg2)
{

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg2 >= 8) ? 1 : 0, 0, (reg1 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x09;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2 & 0x7, reg1 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or32_r_i(uint8_t reg, uint32_t imm32)
{

    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x81;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 1, reg & 0x7);
    memcpy(&opcode_bytes[3], &imm32, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or32_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x0B;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed + sizeof(uint32_t));
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x0B;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed + sizeof(uint32_t);
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

    if (reg3 == REG_ESP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 1);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0,
                   (reg1 >= 8) ? 1 : 0,
                   (reg3 >= 8) ? 1 : 0,
                   (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x0B;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, RM_SIB);
    set_sib(&opcode_bytes[3], 0, reg3 & 0x7, reg2 & 0x7);
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// XOR operations: XOR r,r r,m r,i r,mr

void xor32_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg2 >= 8) ? 1 : 0, 0, (reg1 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x31; // XOR r/m32, r32 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2 & 0x7, reg1 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor32_r_i(uint8_t reg, uint32_t imm32)
{
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x81;                                    // XOR r/m32, imm32 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 6, reg & 0x7); // /6 indicates XOR operation
    memcpy(&opcode_bytes[3], &imm32, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor32_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x33; // XOR r32, r/m32 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed + sizeof(uint32_t));
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, (reg1 >= 8) ? 1 : 0, 0, (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x33; // XOR r32, r/m32 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1 & 0x7, sib_needed ? RM_SIB : (reg2 & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg2 & 0x7);
    }
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed + sizeof(uint32_t);
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

    if (reg3 == REG_ESP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 1);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0,
                   (reg1 >= 8) ? 1 : 0,
                   (reg3 >= 8) ? 1 : 0,
                   (reg2 >= 8) ? 1 : 0);
    opcode_bytes[1] = 0x33; // XOR r32, r/m32 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, RM_SIB);
    set_sib(&opcode_bytes[3], 0, reg3 & 0x7, reg2 & 0x7);
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// NOT operations: NOT r, NOT [r], NOT [r+disp], NOT [r_base+index]

void not32_r(uint8_t reg)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 2, reg & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not32_m(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, sib_needed ? RM_SIB : (reg & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not32_mi(uint8_t reg, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(3 + sib_needed + sizeof(uint32_t));
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 2, sib_needed ? RM_SIB : (reg & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, reg & 0x7);
    }
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed + sizeof(uint32_t);
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not32_mr(uint8_t reg_base, uint8_t reg_index)
{
    cant_use_rx((uint8_t[]){reg_base, reg_index}, 2);

    if (reg_index == REG_ESP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg_base, 1);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, (reg_index >= 8) ? 1 : 0, (reg_base >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, RM_SIB);
    set_sib(&opcode_bytes[3], 0, reg_index & 0x7, reg_base & 0x7);
    opcode_bytes[3 + sib_needed] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// INC operations: INC r, INC [r], INC [r+disp], INC [r_base+index]

void inc32_r(uint8_t reg)
{
    uint8_t regs[] = {reg};
    cant_use_rx(regs, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF;                                    // INC r32
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 0, reg & 0x7); // /0 for INC

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void inc32_m(uint8_t reg)
{
    uint8_t regs[] = {reg};
    cant_use_rx(regs, 1);

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF; // INC r/m32
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, sib_needed ? RM_SIB : (reg & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00; // 1-byte displacement

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void inc32_mi(uint8_t reg, uint32_t offset)
{
    uint8_t regs[] = {reg};
    cant_use_rx(regs, 1);

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF; // INC r/m32
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 0, sib_needed ? RM_SIB : (reg & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg & 0x7);
    }
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void inc32_mr(uint8_t reg_base, uint8_t reg_index)
{
    uint8_t regs[] = {reg_base, reg_index};
    cant_use_rx(regs, 2);

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, (reg_index >= 8) ? 1 : 0, (reg_base >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF; // INC r/m32
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg_index & 0x7, reg_base & 0x7);
    opcode_bytes[4] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 5;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// DEC operations: DEC r, DEC [r], DEC [r+disp], DEC [r_base+index]

void dec32_r(uint8_t reg)
{
    uint8_t regs[] = {reg};
    cant_use_rx(regs, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF;                                    // DEC r32
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 1, reg & 0x7); // /1 for DEC

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void dec32_m(uint8_t reg)
{
    uint8_t regs[] = {reg};
    cant_use_rx(regs, 1);

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF; // DEC r/m32
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 1, sib_needed ? RM_SIB : (reg & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00; // 1-byte displacement

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void dec32_mi(uint8_t reg, uint32_t offset)
{
    uint8_t regs[] = {reg};
    cant_use_rx(regs, 1);

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, 0, (reg >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF; // DEC r/m32
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 1, sib_needed ? RM_SIB : (reg & 0x7));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg & 0x7);
    }
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void dec32_mr(uint8_t reg_base, uint8_t reg_index)
{
    uint8_t regs[] = {reg_base, reg_index};
    cant_use_rx(regs, 2);

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(&opcode_bytes[0], 0, 0, (reg_index >= 8) ? 1 : 0, (reg_base >= 8) ? 1 : 0);
    opcode_bytes[1] = 0xFF; // DEC r/m32
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg_index & 0x7, reg_base & 0x7);
    opcode_bytes[4] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 5;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

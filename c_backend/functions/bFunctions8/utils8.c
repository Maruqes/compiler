#include "bFunctions8.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
#include "../bFunctions32/bFunctions32.h"

void cmp8_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x38; // CMP r/m8, r8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, reg2, reg1);

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void cmp8_r_i(uint8_t reg1, uint8_t imm8)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x80;                               // CMP r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 7, reg1); // /7 indicates CMP operation
    opcode_bytes[2] = imm8;

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void cmp8_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x3A; // CMP r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    opcode_bytes[2 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void cmp8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x3A; // CMP r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[2 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void lshfit8(uint8_t reg, uint8_t imm)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xC0;                              // SHL r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 4, reg); // /4 indicates SHL operation
    opcode_bytes[2] = imm;

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void rshfit8(uint8_t reg, uint8_t imm)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xC0;                              // SHR r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 5, reg); // /5 indicates SHR operation
    opcode_bytes[2] = imm;

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void lshfit8_reg(uint8_t reg1)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xD2;                               // SHL r/m8, CL opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 4, reg1); // /4 indicates SHL operation

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void rshfit8_reg(uint8_t reg1)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xD2;                               // SHR r/m8, CL opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 5, reg1); // /5 indicates SHR operation

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
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

void and8_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x20; // AND r/m8, r8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, reg2, reg1);

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and8_r_i(uint8_t reg, uint8_t imm8)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x80;                              // AND r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 4, reg); // /4 indicates AND operation
    opcode_bytes[2] = imm8;

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and8_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x22; // AND r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    opcode_bytes[2 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x22; // AND r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[2 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void and8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 4);

    if (reg3 == REG_RSP) // ESP RSP ALL SAME SHIT ITS ALL "4"
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x22; // AND r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, RM_SIB);

    set_sib(&opcode_bytes[2], SCALE_1, reg3, reg2);
    opcode_bytes[3] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
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

void or8_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x08; // OR r/m8, r8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, reg2, reg1);

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or8_r_i(uint8_t reg, uint8_t imm8)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x80;                              // OR r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 1, reg); // /1 indicates OR operation
    opcode_bytes[2] = imm8;

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or8_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0A; // OR r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    opcode_bytes[2 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0A; // OR r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[2 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0A; // OR r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[2], SCALE_1, reg3, reg2);
    opcode_bytes[3] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
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

void not8_r(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                              // NOT r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 2, reg); // /2 indicates NOT operation

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not8_m(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                              // NOT r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 2, reg); // /2 indicates NOT operation
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg);
    opcode_bytes[2 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not8_mi(uint8_t reg, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                              // NOT r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, 2, reg); // /2 indicates NOT operation
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg);
    memcpy(&opcode_bytes[2 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not8_mr(uint8_t reg_base, uint8_t reg_index)
{
    cant_use_rx((uint8_t[]){reg_base, reg_index}, 2);

    if (reg_index == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    if (reg_base == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                                 // NOT r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 2, RM_SIB); // /2 indicates NOT operation
    set_sib(&opcode_bytes[2], SCALE_1, reg_index, reg_base);
    opcode_bytes[3] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
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

void xor8_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x30; // XOR r/m8, r8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, reg2, reg1);

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor8_r_i(uint8_t reg, uint8_t imm8)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x80;                              // XOR r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 6, reg); // /6 indicates XOR operation
    opcode_bytes[2] = imm8;

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor8_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x32; // XOR r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    opcode_bytes[2 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x32; // XOR r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[2], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[2 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + sib_needed;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void xor8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x32; // XOR r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[2], SCALE_1, reg3, reg2);
    opcode_bytes[3] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
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

void inc8_r(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE;                              // INC r8
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 0, reg); // /0 for INC

    OpCode new_opcode;
    new_opcode.size = 2;
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

void inc8_m(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg == RM_SIB) // R12 equivalent
    {
        fprintf(stderr, "Error: Cannot use R12 as memory register in 8-bit mode without proper SIB handling.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = (reg == REG_RSP || reg == RM_SIB) ? 1 : 0;
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE; // INC r/m8
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 0, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg);
    }
    opcode_bytes[1 + sib_needed + 1] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed;
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

void inc8_mi(uint8_t reg, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg == RM_SIB) // R12 equivalent
    {
        fprintf(stderr, "Error: Cannot use R12 as memory register in 8-bit mode without proper SIB handling.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = (reg == REG_RSP || reg == RM_SIB) ? 1 : 0;
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE; // INC r/m8
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, 0, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg);
    }
    memcpy(&opcode_bytes[2 + sib_needed], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 6 + sib_needed;
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

void inc8_mr(uint8_t reg_base, uint8_t reg_index)
{
    cant_use_rx((uint8_t[]){reg_base, reg_index}, 2);

    if (reg_base == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as base register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg_index == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as index register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg_base == RM_SIB) // R12 equivalent
    {
        fprintf(stderr, "Error: Cannot use R12 as base register in 8-bit mode without proper SIB handling.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE; // INC r/m8
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 0, RM_SIB);
    set_sib(&opcode_bytes[2], SCALE_1, reg_index, reg_base);
    opcode_bytes[3] = 0x00;

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

// DEC operations: DEC r, DEC [r], DEC [r+disp], DEC [r_base+index]

void dec8_r(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE;                              // DEC r8
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 1, reg); // /1 for DEC

    OpCode new_opcode;
    new_opcode.size = 2;
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

void dec8_m(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg == RM_SIB) // R12 equivalent
    {
        fprintf(stderr, "Error: Cannot use R12 as memory register in 8-bit mode without proper SIB handling.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = (reg == REG_RSP || reg == RM_SIB) ? 1 : 0;
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE; // DEC r/m8
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 1, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg);
    }
    opcode_bytes[1 + sib_needed + 1] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 3 + sib_needed;
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

void dec8_mi(uint8_t reg, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg == RM_SIB) // R12 equivalent
    {
        fprintf(stderr, "Error: Cannot use R12 as memory register in 8-bit mode without proper SIB handling.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = (reg == REG_RSP || reg == RM_SIB) ? 1 : 0;
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE; // DEC r/m8
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, 1, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg);
    }
    memcpy(&opcode_bytes[2 + sib_needed], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 6 + sib_needed;
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

void dec8_mr(uint8_t reg_base, uint8_t reg_index)
{
    cant_use_rx((uint8_t[]){reg_base, reg_index}, 2);

    if (reg_base == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as base register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg_index == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as index register in 8-bit mode.\n");
        exit(EXIT_FAILURE);
    }
    if (reg_base == RM_SIB) // R12 equivalent
    {
        fprintf(stderr, "Error: Cannot use R12 as base register in 8-bit mode without proper SIB handling.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFE; // DEC r/m8
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 1, RM_SIB);
    set_sib(&opcode_bytes[2], SCALE_1, reg_index, reg_base);
    opcode_bytes[3] = 0x00;

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

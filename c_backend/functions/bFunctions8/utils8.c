#include "bFunctions8.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
#include "../bFunctions32/bFunctions32.h"

void cmp8_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // REX: R extends reg2 (ModRM.reg), B extends reg1 (r/m)
    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x38; // CMP r/m8, r8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2, reg1);

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

void cmp8_r_i(uint8_t reg1, uint8_t imm8)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x80;                               // CMP r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 7, reg1); // /7 indicates CMP operation
    opcode_bytes[3] = imm8;

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

void cmp8_r_m(uint8_t reg1, uint8_t reg2)
{
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x3A; // CMP r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    opcode_bytes[3 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
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
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x3A; // CMP r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC0;                              // SHL r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg); // /4 indicates SHL operation
    opcode_bytes[3] = imm;

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

void rshfit8(uint8_t reg, uint8_t imm)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC0;                              // SHR r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg); // /5 indicates SHR operation
    opcode_bytes[3] = imm;

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

void lshfit8_reg(uint8_t reg1)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xD2;                               // SHL r/m8, CL opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg1); // /4 indicates SHL operation

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

void rshfit8_reg(uint8_t reg1)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xD2;                               // SHR r/m8, CL opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg1); // /5 indicates SHR operation

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

// Logical operations: AND, OR, XOR, NOT

void and8_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x20; // AND r/m8, r8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2, reg1);

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

void and8_r_i(uint8_t reg, uint8_t imm8)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x80;                              // AND r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg); // /4 indicates AND operation
    opcode_bytes[3] = imm8;

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

void and8_r_m(uint8_t reg1, uint8_t reg2)
{
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x22; // AND r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    opcode_bytes[3 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
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
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x22; // AND r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x22; // AND r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);

    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[4] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 5;
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
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x08; // OR r/m8, r8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2, reg1);

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

void or8_r_i(uint8_t reg, uint8_t imm8)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x80;                              // OR r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 1, reg); // /1 indicates OR operation
    opcode_bytes[3] = imm8;

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

void or8_r_m(uint8_t reg1, uint8_t reg2)
{
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0A; // OR r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    opcode_bytes[3 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
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
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0A; // OR r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0A; // OR r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[4] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 5;
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
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                              // NOT r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 2, reg); // /2 indicates NOT operation

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

void not8_m(uint8_t reg)
{
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                              // NOT r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, reg); // /2 indicates NOT operation
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg);
    opcode_bytes[3 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
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
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                              // NOT r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 2, reg); // /2 indicates NOT operation
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, (reg_index >= REG_R8) ? 1 : 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                 // NOT r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, RM_SIB); // /2 indicates NOT operation
    set_sib(&opcode_bytes[3], SCALE_1, reg_index, reg_base);
    opcode_bytes[4] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 5;
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
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x30; // XOR r/m8, r8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2, reg1);

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

void xor8_r_i(uint8_t reg, uint8_t imm8)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x80;                              // XOR r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 6, reg); // /6 indicates XOR operation
    opcode_bytes[3] = imm8;

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

void xor8_r_m(uint8_t reg1, uint8_t reg2)
{
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x32; // XOR r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    opcode_bytes[3 + sib_needed] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + sib_needed;
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
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x32; // XOR r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + sib_needed], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x32; // XOR r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[4] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 5;
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
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE;                              // INC r8
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 0, reg); // /0 for INC

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

void inc8_m(uint8_t reg)
{
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
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE; // INC r/m8
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
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

void inc8_mi(uint8_t reg, uint32_t offset)
{
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
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE; // INC r/m8
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 0, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
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

void inc8_mr(uint8_t reg_base, uint8_t reg_index)
{
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, (reg_index >= REG_R8) ? 1 : 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE; // INC r/m8
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg_index, reg_base);
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

void dec8_r(uint8_t reg)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE;                              // DEC r8
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 1, reg); // /1 for DEC

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

void dec8_m(uint8_t reg)
{
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
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE; // DEC r/m8
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 1, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
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

void dec8_mi(uint8_t reg, uint32_t offset)
{
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
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE; // DEC r/m8
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 1, reg);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
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

void dec8_mr(uint8_t reg_base, uint8_t reg_index)
{
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, (reg_index >= REG_R8) ? 1 : 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFE; // DEC r/m8
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg_index, reg_base);
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

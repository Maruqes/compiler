#include "bFunctions64.h"
#include "../functions.h"

// file for cmp shifts push/pop logic(and/or/xor/not)
// irm

/*
cmp entre-> r,r  r,m r,i

*/

void cmp64_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x39;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg1, reg2);

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

void cmp64_r_i(uint8_t reg1, uint32_t imm32)
{
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, 0);
    opcode_bytes[1] = 0x81;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 7, reg1);
    memcpy(&opcode_bytes[3], &imm32, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7;
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

void cmp64_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x3b;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);

    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    opcode_bytes[3 + usa_sib] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void cmp64_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x3b;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);

    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void lshfit64(uint8_t reg, uint8_t imm)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC1;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg);
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

void rshfit64(uint8_t reg, uint8_t imm)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC1;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg);
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

// Shift by CL register
void lshfit64_reg(uint8_t reg)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xD3;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg);

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

// Shift by CL register
void rshfit64_reg(uint8_t reg)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xD3;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg);

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

void push64(uint8_t reg)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    if (reg >= REG_R8)
    {
        reg -= REG_R8;
    }
    opcode_bytes[1] = 0x50 + reg;

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

void push64_m(uint8_t reg)
{
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot push RSP onto the stack.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFF;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 6, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    opcode_bytes[3 + usa_sib] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void push64_mi(uint8_t reg, uint32_t offset)
{
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot push RSP onto the stack.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFF;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 6, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void pop64(uint8_t reg)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    if (reg >= REG_R8)
    {
        reg -= REG_R8;
    }
    opcode_bytes[1] = 0x58 + reg;

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

void pop64_m(uint8_t reg)
{
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot push RSP onto the stack.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8F;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    opcode_bytes[3 + usa_sib] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void pop64_mi(uint8_t reg, uint32_t offset)
{
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot push RSP onto the stack.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8F;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 0, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void and64_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x21;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2, reg1);

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

void and64_r_i(uint8_t reg, uint32_t imm32)
{
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x81;
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg);
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

void and64_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x23;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);

    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    opcode_bytes[3 + usa_sib] = 0x00;

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void and64_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x23;
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);

    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void and64_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    // reg1 = destino, reg2 = base, reg3 = índice
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as base register.\n");
        exit(EXIT_FAILURE);
    }
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as index register.\n");
        exit(EXIT_FAILURE);
    }

    const int need_sib = 1; // vamos usar sempre SIB para [base+index]
    char *opcode_bytes = malloc(4 + need_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1,
                   (reg1 >= REG_R8) ? 1 : 0,
                   (reg3 >= REG_R8) ? 1 : 0,
                   (reg2 >= REG_R8) ? 1 : 0);

    opcode_bytes[1] = 0x23; // AND r64, r/m64
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[3 + need_sib] = 0x00;

    OpCode new_opcode = {.size = 4 + need_sib, .code = opcode_bytes};

    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// OR operations: OR r,r r,m r,i r,mr

void or64_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   (reg2 >= REG_R8) ? 1 : 0,
                   0,
                   (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x09; // OR r/m64, r64
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2, reg1);

    OpCode new_opcode = {.size = 3, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or64_r_i(uint8_t reg, uint32_t imm32)
{
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   0,
                   0,
                   (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x81;                              // OR r64, imm32
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 1, reg); // /1 for OR
    memcpy(&opcode_bytes[3], &imm32, sizeof(uint32_t));

    OpCode new_opcode = {.size = 7, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or64_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   (reg1 >= REG_R8) ? 1 : 0,
                   0,
                   (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0B; // OR r64, r/m64
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    opcode_bytes[3 + usa_sib] = 0x00;

    OpCode new_opcode = {.size = 4 + usa_sib, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or64_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   (reg1 >= REG_R8) ? 1 : 0,
                   0,
                   (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0B; // OR r64, r/m64 with disp32
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset));

    OpCode new_opcode = {.size = 7 + usa_sib, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void or64_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as base register.\n");
        exit(EXIT_FAILURE);
    }
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as index register.\n");
        exit(EXIT_FAILURE);
    }

    const int need_sib = 1; // sempre SIB para [base+index]
    char *opcode_bytes = malloc(4 + need_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   (reg1 >= REG_R8) ? 1 : 0,
                   (reg3 >= REG_R8) ? 1 : 0,
                   (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0B; // OR r64, r/m64
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[3 + need_sib] = 0x00;

    OpCode new_opcode = {.size = 4 + need_sib, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// NOT operations: NOT r, NOT [r], NOT [r+disp], NOT [r_base+index]

void not64_r(uint8_t reg)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   0,
                   0,
                   (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;                              // NOT r64
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 2, reg); // /2

    OpCode new_opcode = {.size = 3, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not64_m(uint8_t reg)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg, 0);
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   0,
                   0,
                   (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7; // NOT r/m64
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    opcode_bytes[3 + usa_sib] = 0x00;

    OpCode new_opcode = {.size = 4 + usa_sib, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not64_mi(uint8_t reg, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg, 0);
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   0,
                   0,
                   (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7; // NOT r/m64
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 2, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset));

    OpCode new_opcode = {.size = 7 + usa_sib, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void not64_mr(uint8_t reg_base, uint8_t reg_index)
{
    if (reg_base == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as base register.\n");
        exit(EXIT_FAILURE);
    }
    if (reg_index == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as index register.\n");
        exit(EXIT_FAILURE);
    }
    const int need_sib = 1;

    char *opcode_bytes = malloc(4 + need_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    set_rex_prefix(opcode_bytes, 1,
                   0,
                   (reg_index >= REG_R8) ? 1 : 0,
                   (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7; // NOT r/m64
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg_index, reg_base);
    opcode_bytes[3 + need_sib] = 0x00;

    OpCode new_opcode = {.size = 4 + need_sib, .code = opcode_bytes};
    OpCode *tmp = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!tmp)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array = tmp;
    op_codes_array[op_codes_array_size++] = new_opcode;
}

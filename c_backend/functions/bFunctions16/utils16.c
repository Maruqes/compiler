#include "bFunctions16.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
#include "../bFunctions32/bFunctions32.h"

void cmp16_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x39; // CMP r/m16, r16 opcode
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

void cmp16_r_i(uint8_t reg1, uint16_t imm16)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                               // 16-bit operand size prefix
    opcode_bytes[1] = 0x81;                               // CMP r/m16, imm16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 7, reg1); // /7 indicates CMP operation
    memcpy(&opcode_bytes[3], &imm16, sizeof(uint16_t));

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

void cmp16_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

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

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x3b; // CMP r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte

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

void cmp16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

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

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x3b; // CMP r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(uint32_t));

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

void lshfit16(uint8_t reg, uint8_t imm)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0xC1;                              // SHL r/m16, imm8 opcode
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

void rshfit16(uint8_t reg, uint8_t imm)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0xC1;                              // SHR r/m16, imm8 opcode
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

void lshfit16_reg(uint8_t reg1)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                               // 16-bit operand size prefix
    opcode_bytes[1] = 0xD3;                               // SHL r/m16, CL opcode
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

void rshfit16_reg(uint8_t reg1)
{

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                               // 16-bit operand size prefix
    opcode_bytes[1] = 0xD3;                               // SHR r/m16, CL opcode
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

void and16_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x21; // AND r/m16, r16 opcode
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

void and16_r_i(uint8_t reg, uint16_t imm16)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0x81;                              // AND r/m16, imm16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 4, reg); // /4 indicates AND operation
    memcpy(&opcode_bytes[3], &imm16, sizeof(uint16_t));

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

void and16_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x23; // AND r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void and16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x23; // AND r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void and16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

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

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x23; // AND r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[4] = 0x00; // Displacement byte (not used in this case)

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

void or16_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x09; // OR r/m16, r16 opcode
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

void or16_r_i(uint8_t reg, uint16_t imm16)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0x81;                              // OR r/m16, imm16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 1, reg); // /1 indicates OR operation
    memcpy(&opcode_bytes[3], &imm16, sizeof(uint16_t));

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

void or16_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x0B; // OR r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void or16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x0B; // OR r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void or16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x0B; // OR r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[4] = 0x00; // Displacement byte (not used in this case)

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

void not16_r(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                              // NOT r/m16 opcode
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

void not16_m(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                              // NOT r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, reg); // /2 indicates NOT operation

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    }
    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void not16_mi(uint8_t reg, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg, 0);
    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                              // NOT r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 2, reg); // /2 indicates NOT operation

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    }
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void not16_mr(uint8_t reg_base, uint8_t reg_index)
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                 // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                                 // NOT r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 2, RM_SIB); // /2 indicates NOT operation
    set_sib(&opcode_bytes[3], SCALE_1, reg_index, reg_base);
    opcode_bytes[4] = 0x00; // Displacement byte (not used in this case)

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

void xor16_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x31; // XOR r/m16, r16 opcode
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

void xor16_r_i(uint8_t reg, uint16_t imm16)
{
    cant_use_rx((uint8_t[]){reg}, 1);

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                              // 16-bit operand size prefix
    opcode_bytes[1] = 0x81;                              // XOR r/m16, imm16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 6, reg); // /6 indicates XOR operation
    memcpy(&opcode_bytes[3], &imm16, sizeof(uint16_t));

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

void xor16_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x33; // XOR r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
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

void xor16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x33; // XOR r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
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

void xor16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
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

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x33; // XOR r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);
    opcode_bytes[4] = 0x00; // Displacement byte (not used in this case)

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

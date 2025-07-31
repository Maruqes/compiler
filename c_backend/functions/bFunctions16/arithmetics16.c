#include "bFunctions16.h"
#include "../functions.h"

// sum sub mul div

void sum16_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x01; // ADD r/m16, r16 opcode
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

void sum16_r_i(uint8_t reg1, uint16_t imm16)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                               // 16-bit operand size prefix
    opcode_bytes[1] = 0x81;                               // ADD r/m16, imm16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 0, reg1); // /0 for ADD
    memcpy(&opcode_bytes[3], &imm16, sizeof(uint16_t));   // 16-bit immediate

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

void sum16_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x03; // ADD r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[3 + usa_sib] = 0x00; // 1-byte displacement (0)

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

void sum16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x03; // ADD r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

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

void sum16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_SP)
    {
        fprintf(stderr, "Error: Cannot use SP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                    // 16-bit operand size prefix
    opcode_bytes[1] = 0x03;                                    // ADD r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB); // Use SIB byte
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);            // [reg2 + reg3*1]
    opcode_bytes[4] = 0x00;                                    // 1-byte displacement (0)

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

void sub16_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x29; // SUB r/m16, r16 opcode
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

void sub16_r_i(uint8_t reg1, uint16_t imm16)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                               // 16-bit operand size prefix
    opcode_bytes[1] = 0x81;                               // SUB r/m16, imm16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg1); // /5 for SUB
    memcpy(&opcode_bytes[3], &imm16, sizeof(uint16_t));   // 16-bit immediate

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

void sub16_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x2B; // SUB r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[3 + usa_sib] = 0x00; // 1-byte displacement (0)

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

void sub16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x2B; // SUB r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

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

void sub16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_SP)
    {
        fprintf(stderr, "Error: Cannot use SP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                    // 16-bit operand size prefix
    opcode_bytes[1] = 0x2B;                                    // SUB r16, r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1, RM_SIB); // Use SIB byte
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);            // [reg2 + reg3*1]
    opcode_bytes[4] = 0x00;                                    // 1-byte displacement (0)

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

void mul16_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF; // IMUL r16, r/m16 opcode
    set_modrm(&opcode_bytes[3], MOD_REG_DIRECT, reg1, reg2);

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

void mul16_r_i(uint8_t reg1, uint16_t imm16)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                  // 16-bit operand size prefix
    opcode_bytes[1] = 0x69;                                  // IMUL r16, r/m16, imm16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg1, reg1); // reg1 = reg1 * imm16
    memcpy(&opcode_bytes[3], &imm16, sizeof(uint16_t));

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

void mul16_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(5 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF; // IMUL r16, r/m16 opcode
    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[4], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[4 + usa_sib] = 0x00; // 1-byte displacement (0)

    OpCode new_opcode;
    new_opcode.size = 5 + usa_sib;
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

void mul16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(8 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // 16-bit operand size prefix
    opcode_bytes[1] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF; // IMUL r16, r/m16 opcode
    set_modrm(&opcode_bytes[3], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[4], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[4 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 8 + usa_sib;
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

void mul16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_SP)
    {
        fprintf(stderr, "Error: Cannot use SP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                    // 16-bit operand size prefix
    opcode_bytes[1] = 0x0F;                                    // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF;                                    // IMUL r16, r/m16 opcode
    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, reg1, RM_SIB); // Use SIB byte
    set_sib(&opcode_bytes[4], SCALE_1, reg3, reg2);            // [reg2 + reg3*1]
    opcode_bytes[5] = 0x00;                                    // 1-byte displacement (0)

    OpCode new_opcode;
    new_opcode.size = 6;
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

void div16_r(uint8_t reg1)
{
    // IDIV r16 - divides DX:AX by reg1
    // Quotient -> AX, Remainder -> DX
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                               // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                               // IDIV r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 7, reg1); // /7 indicates IDIV operation

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

void div16_m(uint8_t reg1)
{
    // IDIV m16 - divides DX:AX by memory operand [reg1]
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                                  // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                                                  // IDIV r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 7, usa_sib ? RM_SIB : reg1); // /7 indicates IDIV

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg1);
    }
    opcode_bytes[3 + usa_sib] = 0x00; // 1-byte displacement (0)

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

void div16_mi(uint8_t reg2, uint32_t offset)
{
    // IDIV m16 - divides DX:AX by memory operand [reg2 + offset]
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                                  // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                                                  // IDIV r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 7, usa_sib ? RM_SIB : reg2); // /7 indicates IDIV

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

void div16_mr(uint8_t reg2, uint8_t reg3)
{
    // IDIV m16 - divides DX:AX by memory operand [reg2 + reg3]
    if (reg3 == REG_SP)
    {
        fprintf(stderr, "Error: Cannot use SP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                                 // 16-bit operand size prefix
    opcode_bytes[1] = 0xF7;                                 // IDIV r/m16 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 7, RM_SIB); // /7 indicates IDIV, use SIB byte
    set_sib(&opcode_bytes[3], SCALE_1, reg3, reg2);         // [reg2 + reg3*1]
    opcode_bytes[4] = 0x00;                                 // 1-byte displacement (0)

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

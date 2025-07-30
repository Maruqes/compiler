#include "bFunctions32.h"
#include "../functions.h"

// sum sub mul div

void sum32_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x01; // ADD r/m32, r32 opcode
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

void sum32_r_i(uint8_t reg1, uint32_t imm32)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x81;                               // ADD r/m32, imm32 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 0, reg1); // /0 for ADD
    memcpy(&opcode_bytes[2], &imm32, sizeof(uint32_t));   // 32-bit immediate

    OpCode new_opcode;
    new_opcode.size = 6;
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

void sum32_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(3 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x03; // ADD r32, r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[2 + usa_sib] = 0x00; // 1-byte displacement (0)

    OpCode new_opcode;
    new_opcode.size = 3 + usa_sib;
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

void sum32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(6 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x03; // ADD r32, r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[2 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + usa_sib;
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

void sum32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_ESP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x03;                                    // ADD r32, r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, RM_SIB); // Use SIB byte
    set_sib(&opcode_bytes[2], SCALE_1, reg3, reg2);            // [reg2 + reg3*1]
    opcode_bytes[3] = 0x00;                                    // 1-byte displacement (0)

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

void sub32_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x29; // SUB r/m32, r32 opcode
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

void sub32_r_i(uint8_t reg1, uint32_t imm32)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x81;                               // SUB r/m32, imm32 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 5, reg1); // /5 for SUB
    memcpy(&opcode_bytes[2], &imm32, sizeof(uint32_t));   // 32-bit immediate

    OpCode new_opcode;
    new_opcode.size = 6;
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

void sub32_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(3 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x2B; // SUB r32, r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[2 + usa_sib] = 0x00; // 1-byte displacement (0)

    OpCode new_opcode;
    new_opcode.size = 3 + usa_sib;
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

void sub32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(6 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x2B; // SUB r32, r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[2 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + usa_sib;
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

void sub32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_ESP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x2B;                                    // SUB r32, r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, RM_SIB); // Use SIB byte
    set_sib(&opcode_bytes[2], SCALE_1, reg3, reg2);            // [reg2 + reg3*1]
    opcode_bytes[3] = 0x00;                                    // 1-byte displacement (0)

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

void mul32_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[1] = 0xAF; // IMUL r32, r/m32 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg1, reg2);

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

void mul32_r_i(uint8_t reg1, uint32_t imm32)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x69;                                  // IMUL r32, r/m32, imm32 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, reg1, reg1); // reg1 = reg1 * imm32
    memcpy(&opcode_bytes[2], &imm32, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6;
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

void mul32_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[1] = 0xAF; // IMUL r32, r/m32 opcode
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

void mul32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[1] = 0xAF; // IMUL r32, r/m32 opcode
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

void mul32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_ESP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;                                    // Two-byte opcode prefix for IMUL
    opcode_bytes[1] = 0xAF;                                    // IMUL r32, r/m32 opcode
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

void div32_r(uint8_t reg1)
{
    // IDIV r32 - divides EDX:EAX by reg1
    // Quotient -> EAX, Remainder -> EDX
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF7;                               // IDIV r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 7, reg1); // /7 indicates IDIV operation

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

void div32_m(uint8_t reg1)
{
    // IDIV m32 - divides EDX:EAX by memory operand [reg1]
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(3 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF7;                                                  // IDIV r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 7, usa_sib ? RM_SIB : reg1); // /7 indicates IDIV

    if (usa_sib)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg1);
    }
    opcode_bytes[2 + usa_sib] = 0x00; // 1-byte displacement (0)

    OpCode new_opcode;
    new_opcode.size = 3 + usa_sib;
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

void div32_mi(uint8_t reg2, uint32_t offset)
{
    // IDIV m32 - divides EDX:EAX by memory operand [reg2 + offset]
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(6 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF7;                                                  // IDIV r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, 7, usa_sib ? RM_SIB : reg2); // /7 indicates IDIV

    if (usa_sib)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[2 + usa_sib], &offset, sizeof(uint32_t));

    OpCode new_opcode;
    new_opcode.size = 6 + usa_sib;
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

void div32_mr(uint8_t reg2, uint8_t reg3)
{
    // IDIV m32 - divides EDX:EAX by memory operand [reg2 + reg3]
    if (reg3 == REG_ESP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF7;                                 // IDIV r/m32 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 7, RM_SIB); // /7 indicates IDIV, use SIB byte
    set_sib(&opcode_bytes[2], SCALE_1, reg3, reg2);         // [reg2 + reg3*1]
    opcode_bytes[3] = 0x00;                                 // 1-byte displacement (0)

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

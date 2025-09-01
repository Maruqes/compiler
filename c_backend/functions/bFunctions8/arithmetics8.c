#include "bFunctions8.h"
#include "../functions.h"
#include "../bFunctions32/bFunctions32.h"
#include "../bFunctions64/bFunctions64.h"

// sum sub mul div

void sum8_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // REX: R extends reg2 (source), B extends reg1 (dest)
    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x00; // ADD r/m8, r8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2 & 0x7, reg1 & 0x7);

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

void sum8_r_i(uint8_t reg1, uint8_t imm8)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x80;                                     // ADD r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 0, reg1 & 0x7); // /0 for ADD
    opcode_bytes[3] = imm8;                                     // 8-bit immediate

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

void sum8_r_m(uint8_t reg1, uint8_t reg2)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x02; // ADD r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, (sib_needed ? RM_SIB : (reg2 & 0x7)));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void sum8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x02; // ADD r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1 & 0x7, (sib_needed ? RM_SIB : (reg2 & 0x7)));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2 & 0x7);
    }
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

void sum8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_RSP)
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

    set_rex_prefix(opcode_bytes, 0,
                   (reg1 >= REG_R8) ? 1 : 0,
                   (reg3 >= REG_R8) ? 1 : 0,
                   (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x02;                                          // ADD r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, RM_SIB); // Use SIB byte
    set_sib(&opcode_bytes[3], SCALE_1, reg3 & 0x7, reg2 & 0x7);      // [reg2 + reg3*1]
    opcode_bytes[4] = 0x00;                                          // 1-byte displacement (0)

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

void sub8_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // REX: R extends reg2 (source), B extends reg1 (dest)
    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x28; // SUB r/m8, r8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg2 & 0x7, reg1 & 0x7);

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

void sub8_r_i(uint8_t reg1, uint8_t imm8)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x80;                                     // SUB r/m8, imm8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg1 & 0x7); // /5 for SUB
    opcode_bytes[3] = imm8;                                     // 8-bit immediate

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

void sub8_r_m(uint8_t reg1, uint8_t reg2)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x2A; // SUB r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, (sib_needed ? RM_SIB : (reg2 & 0x7)));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void sub8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x2A; // SUB r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1 & 0x7, (sib_needed ? RM_SIB : (reg2 & 0x7)));
    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2 & 0x7);
    }
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

void sub8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_RSP)
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

    set_rex_prefix(opcode_bytes, 0,
                   (reg1 >= REG_R8) ? 1 : 0,
                   (reg3 >= REG_R8) ? 1 : 0,
                   (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x2A;                                          // SUB r8, r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg1 & 0x7, RM_SIB); // Use SIB byte
    set_sib(&opcode_bytes[3], SCALE_1, reg3 & 0x7, reg2 & 0x7);      // [reg2 + reg3*1]
    opcode_bytes[4] = 0x00;                                          // 1-byte displacement (0)

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

void mul8_r(uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                     // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg2 & 0x7); // /5 indicates IMUL operation

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

void mul8_m(uint8_t reg2)
{
    // IMUL m8 - multiplies AL by memory operand [reg2]
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                                             // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 5, sib_needed ? RM_SIB : (reg2 & 0x7)); // /5 indicates IMUL

    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void mul8_mi(uint8_t reg2, uint32_t offset)
{
    // IMUL m8 - multiplies AL by memory operand [reg2 + offset]
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                                             // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 5, sib_needed ? RM_SIB : (reg2 & 0x7)); // /5 indicates IMUL

    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2 & 0x7);
    }
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

void mul8_mr(uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }
    // IMUL m8 - multiplies AL by memory operand [reg2 + reg3]
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                     // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 5, RM_SIB);     // /5 indicates IMUL, use SIB byte
    set_sib(&opcode_bytes[3], SCALE_1, reg3 & 0x7, reg2 & 0x7); // [reg2 + reg3*1]
    opcode_bytes[4] = 0x00;                                     // 1-byte displacement (0)

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

void div8_r(uint8_t reg1)
{
    // IDIV r8 - divides AX by reg1
    // Quotient -> AL, Remainder -> AH
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                     // IDIV r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 7, reg1 & 0x7); // /7 indicates IDIV operation

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

void div8_m(uint8_t reg1)
{
    // IDIV m8 - divides AX by memory operand [reg1]
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg1, 0);
    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                                             // IDIV r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 7, sib_needed ? RM_SIB : (reg1 & 0x7)); // /7 indicates IDIV

    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg1 & 0x7);
    }
    opcode_bytes[3 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void div8_mi(uint8_t reg2, uint32_t offset)
{
    // IDIV m8 - divides AX by memory operand [reg2 + offset]
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                                             // IDIV r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 7, sib_needed ? RM_SIB : (reg2 & 0x7)); // /7 indicates IDIV

    if (sib_needed)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2 & 0x7);
    }
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

void div8_mr(uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }
    // IDIV m8 - divides AX by memory operand [reg2 + reg3]
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 0, 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF6;                                     // IDIV r/m8 opcode
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 7, RM_SIB);     // /7 indicates IDIV, use SIB byte
    set_sib(&opcode_bytes[3], SCALE_1, reg3 & 0x7, reg2 & 0x7); // [reg2 + reg3*1]
    opcode_bytes[4] = 0x00;                                     // 1-byte displacement (0)

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

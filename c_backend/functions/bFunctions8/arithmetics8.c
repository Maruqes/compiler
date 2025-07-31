#include "bFunctions8.h"
#include "../functions.h"
#include "../bFunctions32/bFunctions32.h"
#include "../bFunctions64/bFunctions64.h"

// sum sub mul div

void sum8_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x00; // ADD r/m8, r8 opcode
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

void sum8_r_i(uint8_t reg1, uint8_t imm8)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x80;                               // ADD r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 0, reg1); // /0 for ADD
    opcode_bytes[2] = imm8;                               // 8-bit immediate

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

void sum8_r_m(uint8_t reg1, uint8_t reg2)
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

    opcode_bytes[0] = 0x02; // ADD r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, reg2);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[2 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void sum8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
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

    opcode_bytes[0] = 0x02; // ADD r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, reg2);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
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

void sum8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
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

    opcode_bytes[0] = 0x02;                                    // ADD r8, r/m8 opcode
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

void sub8_r_r(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x28; // SUB r/m8, r8 opcode
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

void sub8_r_i(uint8_t reg1, uint8_t imm8)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x80;                               // SUB r/m8, imm8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 5, reg1); // /5 for SUB
    opcode_bytes[2] = imm8;                               // 8-bit immediate

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

void sub8_r_m(uint8_t reg1, uint8_t reg2)
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

    opcode_bytes[0] = 0x2A; // SUB r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, reg1, reg2);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[2 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void sub8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
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

    opcode_bytes[0] = 0x2A; // SUB r8, r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, reg1, reg2);
    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
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

void sub8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
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

    opcode_bytes[0] = 0x2A;                                    // SUB r8, r/m8 opcode
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

void mul8_r(uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg2}, 1);

    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                               // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_REG_DIRECT, 5, reg2); // /5 indicates IMUL operation

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

void mul8_m(uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg2}, 1);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    // IMUL m8 - multiplies AL by memory operand [reg2]
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                                                     // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 5, sib_needed ? RM_SIB : reg2); // /5 indicates IMUL

    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
    opcode_bytes[2 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void mul8_mi(uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg2}, 1);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    // IMUL m8 - multiplies AL by memory operand [reg2 + offset]
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                                                     // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, 5, sib_needed ? RM_SIB : reg2); // /5 indicates IMUL

    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
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

void mul8_mr(uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg2, reg3}, 2);

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

    // IMUL m8 - multiplies AL by memory operand [reg2 + reg3]
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                                 // IMUL r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 5, RM_SIB); // /5 indicates IMUL, use SIB byte
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

void div8_r(uint8_t reg1)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    // IDIV r8 - divides AX by reg1
    // Quotient -> AL, Remainder -> AH
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                               // IDIV r/m8 opcode
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

void div8_m(uint8_t reg1)
{
    cant_use_rx((uint8_t[]){reg1}, 1);

    if (reg1 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    // IDIV m8 - divides AX by memory operand [reg1]
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg1, 0);
    char *opcode_bytes = malloc(3 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                                                     // IDIV r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_1BYTE_DISP, 7, sib_needed ? RM_SIB : reg1); // /7 indicates IDIV

    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg1);
    }
    opcode_bytes[2 + sib_needed] = 0x00; // 1-byte displacement (0)

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

void div8_mi(uint8_t reg2, uint32_t offset)
{
    cant_use_rx((uint8_t[]){reg2}, 1);

    if (reg2 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a memory register.\n");
        exit(EXIT_FAILURE);
    }

    // IDIV m8 - divides AX by memory operand [reg2 + offset]
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    char *opcode_bytes = malloc(6 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                                                     // IDIV r/m8 opcode
    set_modrm(&opcode_bytes[1], MOD_4BYTE_DISP, 7, sib_needed ? RM_SIB : reg2); // /7 indicates IDIV

    if (sib_needed)
    {
        set_sib(&opcode_bytes[2], SCALE_1, RM_SIB, reg2);
    }
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

void div8_mr(uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg2, reg3}, 2);

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

    // IDIV m8 - divides AX by memory operand [reg2 + reg3]
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF6;                                 // IDIV r/m8 opcode
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

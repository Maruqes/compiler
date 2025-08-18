#include "bFunctions64.h"
#include "../functions.h"

// sum sub mul div

void sum64_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg2 high bit, X=0, B=reg1 high bit
    set_rex_prefix(opcode_bytes, 1, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x01; // ADD r/m64, r64 opcode
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

void sum64_r_i(uint8_t reg1, uint32_t imm64)
{
    // Use 32-bit immediate which gets sign-extended to 64-bit
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x81;                               // ADD r/m64, imm32 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 0, reg1); // /0 indicates ADD operation
    uint32_t imm32 = (uint32_t)imm64;
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

void sum64_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x03; // ADD r64, r/m64 opcode
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

void sum64_r_mi(uint8_t reg1, uint8_t reg2, uint64_t offset)
{
    // For now, limit offset to 32-bit since x86-64 displacements are typically 32-bit
    if (offset > UINT32_MAX)
    {
        perror("Offset > UINT32_MAX not supported in sum64_r_mi");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    uint32_t offset32 = (uint32_t)offset;

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x03; // ADD r64, r/m64 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[3 + usa_sib], &offset32, sizeof(uint32_t));

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

void sum64_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=reg3 high bit, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x03;                                    // ADD r64, r/m64 opcode
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

void sub64_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg2 high bit, X=0, B=reg1 high bit
    set_rex_prefix(opcode_bytes, 1, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x29; // SUB r/m64, r64 opcode
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

void sub64_r_i(uint8_t reg1, uint64_t imm64)
{
    // For 64-bit immediate, we need to check if it fits in 32 bits
    if (imm64 <= UINT32_MAX)
    {
        // Use 32-bit immediate which gets sign-extended to 64-bit
        char *opcode_bytes = malloc(7);
        if (!opcode_bytes)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        set_rex_prefix(opcode_bytes, 1, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
        opcode_bytes[1] = 0x81;                               // SUB r/m64, imm32 opcode
        set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, 5, reg1); // /5 indicates SUB operation
        uint32_t imm32 = (uint32_t)imm64;
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
    else
    {
        // For true 64-bit immediate, use MOV + SUB sequence
        // This is a common approach since x86-64 doesn't have 64-bit immediate SUB
        perror("64-bit immediate values > UINT32_MAX not supported in sub64_r_i");
        exit(EXIT_FAILURE);
    }
}

void sub64_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x2B; // SUB r64, r/m64 opcode
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

void sub64_r_mi(uint8_t reg1, uint8_t reg2, uint64_t offset)
{
    // For now, limit offset to 32-bit since x86-64 displacements are typically 32-bit
    if (offset > UINT32_MAX)
    {
        perror("Offset > UINT32_MAX not supported in sub64_r_mi");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    uint32_t offset32 = (uint32_t)offset;

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x2B; // SUB r64, r/m64 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[3 + usa_sib], &offset32, sizeof(uint32_t));

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

void sub64_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=reg3 high bit, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x2B;                                    // SUB r64, r/m64 opcode
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

void mul64_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF; // IMUL r64, r/m64 opcode
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

void mul64_r_i(uint8_t reg1, uint32_t imm64)
{

    // Use 32-bit immediate which gets sign-extended to 64-bit
    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x69;                                  // IMUL r64, r/m64, imm32 opcode
    set_modrm(&opcode_bytes[2], MOD_REG_DIRECT, reg1, reg1); // reg1 = reg1 * imm32
    uint32_t imm32 = (uint32_t)imm64;
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

void mul64_r_m(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg2, 0);

    char *opcode_bytes = malloc(5 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF; // IMUL r64, r/m64 opcode
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

void mul64_r_mi(uint8_t reg1, uint8_t reg2, uint64_t offset)
{
    // For now, limit offset to 32-bit since x86-64 displacements are typically 32-bit
    if (offset > UINT32_MAX)
    {
        perror("Offset > UINT32_MAX not supported in mul64_r_mi");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    uint32_t offset32 = (uint32_t)offset;

    char *opcode_bytes = malloc(8 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0F; // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF; // IMUL r64, r/m64 opcode
    set_modrm(&opcode_bytes[3], MOD_4BYTE_DISP, reg1, usa_sib ? RM_SIB : reg2);

    if (usa_sib)
    {
        set_sib(&opcode_bytes[4], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[4 + usa_sib], &offset32, sizeof(uint32_t));

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

void mul64_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=reg1 high bit, X=reg3 high bit, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, (reg1 >= REG_R8) ? 1 : 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x0F;                                    // Two-byte opcode prefix for IMUL
    opcode_bytes[2] = 0xAF;                                    // IMUL r64, r/m64 opcode
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

void div64_r(uint8_t reg1)
{
    // IDIV r64 - divides RDX:RAX by reg1
    // Quotient -> RAX, Remainder -> RDX
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=0, X=0, B=reg1 high bit
    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;                               // IDIV r/m64 opcode
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

void div64_m(uint8_t reg1)
{
    // IDIV m64 - divides RDX:RAX by memory operand [reg1]
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=0, X=0, B=reg1 high bit
    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;                                                  // IDIV r/m64 opcode
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

void div64_mi(uint8_t reg2, uint64_t offset)
{
    // IDIV m64 - divides RDX:RAX by memory operand [reg2 + offset]
    if (offset > UINT32_MAX)
    {
        perror("Offset > UINT32_MAX not supported in div64_mi");
        exit(EXIT_FAILURE);
    }

    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg2, 0);
    uint32_t offset32 = (uint32_t)offset;

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=0, X=0, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;                                                  // IDIV r/m64 opcode
    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 7, usa_sib ? RM_SIB : reg2); // /7 indicates IDIV

    if (usa_sib)
    {
        set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg2);
    }
    memcpy(&opcode_bytes[3 + usa_sib], &offset32, sizeof(uint32_t));

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

void div64_mr(uint8_t reg2, uint8_t reg3)
{
    // IDIV m64 - divides RDX:RAX by memory operand [reg2 + reg3]
    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix: W=1 (64-bit), R=0, X=reg3 high bit, B=reg2 high bit
    set_rex_prefix(opcode_bytes, 1, 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xF7;                                 // IDIV r/m64 opcode
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

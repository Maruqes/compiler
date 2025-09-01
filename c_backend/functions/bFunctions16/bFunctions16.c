#include "bFunctions16.h"
#include "../functions.h"

/*
i-immediate
r-register
m-memory

functions:

mov16 r, i  :D
mov16 r, m  :D
mov16 r, mi :D
mov16 r, mr :D
mov16 r, r  :D

mov16 m, i  :D
mov16 m, r  :D

mov16 mi, i :D
mov16 mi, r :D

mov16 mr, i
mov16 mr, r
*/

void set_66_prefix(char *opcode_bytes)
{
    opcode_bytes[0] = 0x66; // Prefix for 16-bit operand size
}

void mov16_r_i(uint8_t reg_code, uint16_t value)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes);
    // REX: B extends reg_code for r8w–r15w (W=0 for 16-bit (0x66 used))
    set_rex_prefix(&opcode_bytes[1], 0, 0, 0, (reg_code >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0xB8 + (reg_code & 0x7); // MOV r16, imm16
    memcpy(&opcode_bytes[3], &value, sizeof(uint16_t));

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

// mov16 r [64r]
void mov16_r_m(uint8_t reg, uint8_t mem_reg)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, mem_reg, 0);

    char *opcode_bytes = malloc(5 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes); // Set the 66 prefix for 32-bit operand size
    set_rex_prefix(&opcode_bytes[1], 0, (reg >= REG_R8) ? 1 : 0, 0, (mem_reg >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0x8B;

    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, reg & 0x7, mem_reg & 0x7);
    set_sib(&opcode_bytes[4], 0, RM_SIB, mem_reg & 0x7);
    opcode_bytes[4 + sib_needed] = 0x00; // Displacement byte (not used in this case)

    OpCode new_opcode;
    new_opcode.size = 5 + sib_needed;
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

// mov16 r16 [64r + offset]
void mov16_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg_base, 0);

    char *opcode_bytes = malloc(8 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes); // Set the 66 prefix for 32-bit operand size
    set_rex_prefix(&opcode_bytes[1], 0, (reg_dest >= REG_R8) ? 1 : 0, 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0x8B;

    set_modrm(&opcode_bytes[3], MOD_4BYTE_DISP, reg_dest & 0x7, reg_base & 0x7);
    set_sib(&opcode_bytes[4], 0, RM_SIB, reg_base & 0x7);
    memcpy(&opcode_bytes[4 + sib_needed], &offset, sizeof(int32_t)); // Copy the offset

    OpCode new_opcode;
    new_opcode.size = 8 + sib_needed;
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

// mov 16r [64r + 64r]
void mov16_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
    if (reg_offset == REG_SP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg_base, 1);

    char *opcode_bytes = malloc(5 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes); // Set the 66 prefix for 32-bit operand size
    set_rex_prefix(&opcode_bytes[1], 0,
                   (reg >= REG_R8) ? 1 : 0,
                   (reg_offset >= REG_R8) ? 1 : 0,
                   (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0x8B;

    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, reg & 0x7, RM_SIB);
    set_sib(&opcode_bytes[4], 0, reg_offset & 0x7, reg_base & 0x7);
    opcode_bytes[4 + sib_needed] = 0x00; // Displacement byte (not used in this case)

    OpCode new_opcode;
    new_opcode.size = 5 + sib_needed;
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

void mov16_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(4);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes); // Set the 66 prefix for 32-bit operand size
    set_rex_prefix(&opcode_bytes[1], 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0x89;
    set_modrm(&opcode_bytes[3], MOD_REG_DIRECT, reg2 & 0x7, reg1 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 4;
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

// mov16 [64r], imm16
void mov16_m_i(uint8_t reg1, uint16_t value)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(7 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes);
    set_rex_prefix(&opcode_bytes[1], 0, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0xC7;

    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, 0, reg1 & 0x7);
    set_sib(&opcode_bytes[4], SCALE_1, RM_SIB, reg1 & 0x7);

    opcode_bytes[4 + sib_needed] = 0x00;                             // Displacement byte (not used in this case)
    memcpy(&opcode_bytes[5 + sib_needed], &value, sizeof(uint16_t)); // Copy the immediate value

    OpCode new_opcode;
    new_opcode.size = 7 + sib_needed;
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

// mov16 [64r], reg16
void mov16_m_r(uint8_t reg1, uint8_t reg2)
{
    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(5 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes); // Set the 66 prefix for 32-bit operand size
    set_rex_prefix(&opcode_bytes[1], 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0x89;

    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, reg2 & 0x7, reg1 & 0x7);
    set_sib(&opcode_bytes[4], SCALE_1, RM_SIB, reg1 & 0x7);
    opcode_bytes[4 + sib_needed] = 0x00; // Displacement byte (not used in this case)

    OpCode new_opcode;
    new_opcode.size = 5 + sib_needed;
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

// mov16 [64r + imm32], imm16
void mov16_mi_i(uint8_t reg, int32_t offset, uint16_t value)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(10 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes);
    set_rex_prefix(&opcode_bytes[1], 0, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0xC7;

    set_modrm(&opcode_bytes[3], MOD_4BYTE_DISP, 0, reg & 0x7);
    set_sib(&opcode_bytes[4], SCALE_1, RM_SIB, reg & 0x7);
    memcpy(&opcode_bytes[4 + sib_needed], &offset, sizeof(int32_t)); // Copy the offset
    memcpy(&opcode_bytes[8 + sib_needed], &value, sizeof(uint16_t)); // Copy the immediate value

    OpCode new_opcode;
    new_opcode.size = 10 + sib_needed;
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

// mov16 [64r + imm32], reg16
void mov16_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    int sib_needed = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(8 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes);
    set_rex_prefix(&opcode_bytes[1], 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0x89;

    set_modrm(&opcode_bytes[3], MOD_4BYTE_DISP, reg2 & 0x7, reg & 0x7);
    set_sib(&opcode_bytes[4], SCALE_1, RM_SIB, reg & 0x7);
    memcpy(&opcode_bytes[4 + sib_needed], &offset, sizeof(int32_t)); // Copy the offset

    OpCode new_opcode;
    new_opcode.size = 8 + sib_needed;
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

// mov16 [64r + 64r], imm16
void mov16_mr_i(uint8_t reg, uint8_t reg2, uint16_t value)
{
    if (reg2 == REG_SP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = 1;

    char *opcode_bytes = malloc(8);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes);
    set_rex_prefix(&opcode_bytes[1], 0, 0, (reg2 >= REG_R8) ? 1 : 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0xC7;

    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, 0, RM_SIB);
    set_sib(&opcode_bytes[4], SCALE_1, reg2 & 0x7, reg & 0x7);
    opcode_bytes[5] = 0x00;                             // Displacement byte (not used in this case)
    memcpy(&opcode_bytes[6], &value, sizeof(uint16_t)); // Copy the immediate value

    OpCode new_opcode;
    new_opcode.size = 8;
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

// mov [64r + 64r], reg16
void mov16_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
    if (reg2 == REG_SP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = 1;

    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_66_prefix(opcode_bytes);
    set_rex_prefix(&opcode_bytes[1], 0, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[2] = 0x89;

    set_modrm(&opcode_bytes[3], MOD_1BYTE_DISP, reg3 & 0x7, RM_SIB);
    set_sib(&opcode_bytes[4], SCALE_1, reg2 & 0x7, reg_base & 0x7);
    opcode_bytes[5] = 0x00; // Displacement byte (not used in this case)

    OpCode new_opcode;
    new_opcode.size = 6;
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
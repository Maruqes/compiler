#include "jumps.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"

// RIP = RIP + 8-bit displacement sign extended to 64-bits.
// does not know if it works
void jump_short(uint8_t offset)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xEB; // Short jump opcode
    opcode_bytes[1] = offset;

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

// RIP = RIP + 32-bit displacement sign extended to 64-bits.
// does not know if it works
void jump_near(uint32_t offset)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xE9; // Near jump opcode
    memcpy(&opcode_bytes[1], &offset, 4);

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

// RIP = 64-Bit offset from register or memory.
void jump_reg(uint8_t reg)
{
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a register for jump_reg.\n");
        exit(EXIT_FAILURE);
    }

    int sib_needed = precisa_sib(MOD_1BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(4 + sib_needed);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFF;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 4, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    opcode_bytes[3 + sib_needed] = 0x00;

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

void jmp(char *name)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xE9;

    memset(&opcode_bytes[1], 0, 4); // Placeholder for address
    add_label_jump(name, &opcode_bytes[1], 5);

    OpCode new_opcode;
    new_opcode.size = 5; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// jmps to add -> je/jz  jne/jnz  jg/jnle  jl/jnge  jge/jnl  jle/jng ja/jnbe  jb/jnae  jae/jnb  jbe/jna

// je = jump if equal
void jcc(char *name, uint8_t condition)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = condition;

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address
    add_label_jump(name, &opcode_bytes[2], 6);

    OpCode new_opcode;
    new_opcode.size = 6; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

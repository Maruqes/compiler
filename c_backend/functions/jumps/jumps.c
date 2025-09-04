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
                            // Load the absolute address of a label into a register using RIP-relative LEA.
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
        perror("Failed to reallo\te memory for op_codes_array");
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

void call(char *name)
{
    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xE8;

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

void call_raw_address(uint8_t reg)
{
    // Encode: REX.W + FF /2 r/m64 with ModRM mod=11 (register direct)
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // REX.W=1, set B if high register is used
    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xFF; // Group 5 instructions (INC/DEC/CALL/JMP/PUSH), we use /2 for CALL
    set_modrm((uint8_t *)&opcode_bytes[2], MOD_REG_DIRECT, 2 /* /2 = CALL */, reg & 0x7);

    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Load the absolute address of a label into a register using RIP-relative LEA.
// Encoding: REX.W + 0x8D + ModRM(mod=00, reg=dest, rm=101) + disp32
// disp32 = target - (next RIP) and is fixed later by add_label_jump/fix_all_labels.
void create_label_reference(uint8_t reg, char *name)
{
    if (reg == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use RSP as a register for createLabelReference.\n");
        exit(EXIT_FAILURE);
    }

    char *opcode_bytes = malloc(7);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // REX.W=1, R extends 'reg' if needed; X=0, B=0 (RIP-relative has no base reg)
    set_rex_prefix(opcode_bytes, 1, (reg >= REG_R8) ? 1 : 0, 0, 0);
    opcode_bytes[1] = 0x8D; // LEA
    // ModRM: mod=00 (no base), reg=dest, rm=101 (RIP-relative)
    set_modrm((uint8_t *)&opcode_bytes[2], MOD_NO_DISP, reg & 0x7, 0x5);

    // Placeholder disp32 to be patched to rel32 by fixups
    memset(&opcode_bytes[3], 0, 4);
    add_label_jump(name, &opcode_bytes[3], 7);

    OpCode new_opcode;
    new_opcode.size = 7; // Total instruction size
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
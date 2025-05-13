#include "bFunctions64.h"
#include "../functions.h"

// General Purpose Registers (0-7)
#define REG_RAX 0x0
#define REG_RCX 0x1
#define REG_RDX 0x2
#define REG_RBX 0x3
#define REG_RSP 0x4
#define REG_RBP 0x5
#define REG_RSI 0x6
#define REG_RDI 0x7

// Extended Registers (8-15)
#define REG_R8 0x8
#define REG_R9 0x9
#define REG_R10 0xA
#define REG_R11 0xB
#define REG_R12 0xC
#define REG_R13 0xD
#define REG_R14 0xE
#define REG_R15 0xF

// Special Registers (Use with caution, not for general mov instructions)
#define REG_RIP 0x10
#define REG_RFLAGS 0x11

/*
0100 - Fixos, identificam que é um prefixo REX (0x40 a 0x4F).

W (bit 3) - Define se o operando é de 64 bits (1 para 64 bits, 0 para 32/16/8 bits).

R (bit 2) - Extende o campo de registo no ModR/M.

X (bit 1) - Extende o campo de índice no byte SIB.

B (bit 0) - Extende o campo de base no ModR/M ou SIB.
*/
// Helper function to set REX prefix for 64-bit operations
void set_rex_prefix(char *opcode_bytes, uint8_t w, uint8_t r, uint8_t x, uint8_t b)
{
    // REX prefix: 0x4X where X is composed of W, R, X, B bits
    opcode_bytes[0] = 0x40 | (w << 3) | (r << 2) | (x << 1) | b;
}

void mov64_r_i(uint8_t reg_code, uint64_t value)
{
    if (reg_code == REG_RSP)
    {
        perror("Cannot move immediate value into RSP");
        exit(EXIT_FAILURE);
    }

    // Allocate 10 bytes for REX + opcode + immediate (1 + 1 + 8)
    char *opcode_bytes = malloc(10);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix, handle high registers correctly
    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg_code >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xB8 + (reg_code & 0x7); // Opcode for 'mov r64, imm64'

    // Copy the 8-byte immediate value
    memcpy(&opcode_bytes[2], &value, sizeof(value));

    // Create the OpCode object
    OpCode new_opcode;
    new_opcode.size = 10;
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

void mov64_r_r(uint8_t reg1, uint8_t reg2)
{
    // Allocate 3 bytes for REX + opcode
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix, handle high registers correctly
    set_rex_prefix(opcode_bytes, 0, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x89;                                      // Opcode for 'mov r/m64, r64'
    opcode_bytes[2] = 0xC0 + (reg1 & 0x7) + ((reg2 & 0x7) << 3); // ModR/M byte

    // Create the OpCode object
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

// Function for the SYSCALL instruction (64-bit)
void syscall_instruction()
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F; // First byte of syscall opcode
    opcode_bytes[1] = 0x05; // Second byte of syscall opcode

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move from memory to a 64-bit register
void mov64_r_m(uint8_t reg, uint8_t mem_reg)
{
    // Under development
}

// Function to move from memory with immediate offset to a 64-bit register
void mov64_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
    // Under development
}

// Function to move from memory with register offset to a 64-bit register
void mov64_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
    // Under development
}

// Function to move immediate to memory
void mov64_m_i(uint8_t reg1, uint64_t value)
{
    // Under development
}

// Function to move register to memory
void mov64_m_r(uint8_t reg1, uint8_t reg2)
{
    // Under development
}

// Function to move immediate to memory with immediate offset
void mov64_mi_i(uint8_t reg, int32_t offset, uint64_t value)
{
    // Under development
}

// Function to move register to memory with immediate offset
void mov64_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    // Under development
}

// Function to move immediate to memory with register offset
void mov64_mr_i(uint8_t reg, uint8_t reg2, uint64_t value)
{
    // Under development
}

// Function to move register to memory with register offset
void mov64_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
    // Under development
}

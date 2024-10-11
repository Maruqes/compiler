#include "arithmetic.h"
#include "../functions/functions.h"

void add_eax(uint32_t value)
{
    char *opcode_bytes = malloc(5); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x05;
    memcpy(&opcode_bytes[1], &value, sizeof(value)); // Copy the 4-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 5; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the opcode array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void add(uint8_t reg, uint32_t value)
{
    if (reg == REG_EAX)
    {
        add_eax(value);
        return;
    }

    char *opcode_bytes = malloc(6); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x81;                          // Opcode for 'add r/m32, imm32'
    opcode_bytes[1] = 0xC0 + reg;                    // ModR/M byte for 'r/m32, imm32'
    memcpy(&opcode_bytes[2], &value, sizeof(value)); // Copy the 4-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 6; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the opcode array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void add_reg32(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x01;
    opcode_bytes[1] = 0xC0 + reg1 + (reg2 * 8);

    OpCode new_opcode;
    new_opcode.size = 2; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the opcode array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}
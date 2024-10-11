#include "jumps.h"
#include "../functions/functions.h"

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

    add_fixup(op_codes_array_size, name, 1, 0, 5); // Offset is 1 due to opcode

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

    add_fixup(op_codes_array_size, name, 1, 0, 5); // Offset is 1 due to opcode

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

void jump_if_equal(char *name)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = 0x84;

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    add_fixup(op_codes_array_size, name, 2, 0, 6); // Offset is 2 due to opcode

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

void jump_if_not_equal(char *name)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = 0x85;

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    add_fixup(op_codes_array_size, name, 2, 0, 6); // Offset is 2 due to opcode

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

void jump_if_greater(char *name)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = 0x8F;

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    add_fixup(op_codes_array_size, name, 2, 0, 6); // Offset is 2 due to opcode

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

void jump_if_greater_or_equal(char *name)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = 0x8D;

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    add_fixup(op_codes_array_size, name, 2, 0, 6); // Offset is 2 due to opcode

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

void jump_if_less(char *name)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = 0x8C;

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    add_fixup(op_codes_array_size, name, 2, 0, 6); // Offset is 2 due to opcode

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

void jump_if_less_or_equal(char *name)
{
    char *opcode_bytes = malloc(6);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = 0x8E;

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    add_fixup(op_codes_array_size, name, 2, 0, 6); // Offset is 2 due to opcode

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
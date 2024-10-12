#include "push_pop.h"
#include "../functions/functions.h"
#include "strings.h"

void push_reg(int reg)
{
    int opcode = 0x50 + reg;
    char *opcode_bytes = malloc(1);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = opcode;

    OpCode new_opcode;
    new_opcode.size = 1;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void push_eax()
{
    push_reg(0);
}

void push_ebx()
{
    push_reg(3);
}

void push_ecx()
{
    push_reg(1);
}

void push_edx()
{
    push_reg(2);
}

void push_esi()
{
    push_reg(6);
}

void push_edi()
{
    push_reg(7);
}

void push_ebp()
{
    push_reg(5);
}

void pre_push_symbol_address()
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x68;         // Opcode for 'push imm32'
    memset(&opcode_bytes[1], 0, 4); // Placeholder for address

    OpCode new_opcode;
    new_opcode.size = 5; // 1-byte opcode + 4-byte immediate value
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void push_symbol_address(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name, 1, 0, 0);
    pre_push_symbol_address();
}

void pre_push_symbol()
{
    char *opcode_bytes = malloc(6); // 1 opcode + 1 ModR/M + 4 displacement
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xFF; // Opcode for 'push [r/m32]'
    opcode_bytes[1] = 0x35;
    memset(&opcode_bytes[2], 0, 4);

    OpCode new_opcode;
    new_opcode.size = 6; // Total size of the instruction
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void push_symbol(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name, 2, 0, 0);
    pre_push_symbol();
}

void pop_reg(int reg)
{
    int opcode = 0x58 + reg;
    char *opcode_bytes = malloc(1);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = opcode;

    OpCode new_opcode;
    new_opcode.size = 1;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void pop_eax()
{
    pop_reg(0);
}

void pop_ebx()
{
    pop_reg(3);
}

void pop_ecx()
{
    pop_reg(1);
}

void pop_edx()
{
    pop_reg(2);
}

void pop_esi()
{
    pop_reg(6);
}

void pop_edi()
{
    pop_reg(7);
}

void pop_ebp()
{
    pop_reg(5);
}

void pre_pop_symbol()
{
    char *opcode_bytes = malloc(6); // 1 opcode + 1 ModR/M + 4 displacement
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x8F; // Opcode for 'pop [r/m32]'
    opcode_bytes[1] = 0x05;
    memset(&opcode_bytes[2], 0, 4);

    OpCode new_opcode;
    new_opcode.size = 6; // Total size of the instruction
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void pop_symbol(char *symbol_name)
{
    pre_pop_symbol();                                         // Generate the 'pop [symbol]' instruction
    add_fixup(op_codes_array_size - 1, symbol_name, 2, 0, 0); // Fixup at offset 2
}

void pusha()
{
    char *opcode_bytes = malloc(1);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x60; // Opcode for 'pusha'

    OpCode new_opcode;
    new_opcode.size = 1; // Total instruction size
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

void popa()
{
    char *opcode_bytes = malloc(1);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x61; // Opcode for 'pusha'

    OpCode new_opcode;
    new_opcode.size = 1; // Total instruction size
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
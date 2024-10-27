#include "arithmetic.h"
#include "../functions/functions.h"
#include "../push_pop/push_pop.h"
#include "../mov_reg_reg/mov_reg_reg.h"

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

// sub
void sub_eax(uint32_t value)
{
    char *opcode_bytes = malloc(5); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x2d;
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

void sub(uint8_t reg, uint32_t value)
{
    if (reg == REG_EAX)
    {
        sub_eax(value);
        return;
    }

    char *opcode_bytes = malloc(6); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x81;                          // Opcode for 'add r/m32, imm32'
    opcode_bytes[1] = 0xE8 + reg;                    // ModR/M byte for 'r/m32, imm32'
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

void sub_reg32(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x29;
    opcode_bytes[1] = 0xC0 + reg1 + (reg2 * 8);

    OpCode new_opcode;
    new_opcode.size = 2; // Total instruction size
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// mul
void mul_reg32(uint8_t reg1, uint8_t reg2)
{
    if (reg1 == REG_EAX || reg2 == REG_EAX)
    {
        perror("Multiplication with REG_EAX is not allowed it need to be cleared before multiplication");
        exit(EXIT_FAILURE);
    }
    push_eax();
    mov_reg32_reg32(REG_EAX, reg1);

    char *opcode_bytes = malloc(2); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xf7;
    opcode_bytes[1] = 0xE0 + reg2;

    OpCode new_opcode;
    new_opcode.size = 2; // Total instruction size
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;

    mov_reg32_reg32(reg1, REG_EAX);

    pop_eax();
}

// if you need the reminder change the code it should be in edx but look at the pop(edx)
void div_reg32(uint8_t reg1, uint8_t reg2)
{
    if (reg2 == REG_EAX || reg1 == REG_EAX || reg2 == REG_EDX || reg1 == REG_EDX)
    {
        perror("Division with EAX or EDX is not allowed it need to be cleared before division");
        exit(EXIT_FAILURE);
    }

    push_eax();
    push_edx();

    mov_edx(0);

    mov_reg32_reg32(REG_EAX, reg1);

    char *opcode_bytes = malloc(2); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xf7;
    opcode_bytes[1] = 0xF0 + reg2;

    OpCode new_opcode;
    new_opcode.size = 2; // Total instruction size
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;

    mov_reg32_reg32(reg1, REG_EAX);

    pop_edx();
    pop_eax();
}

void inc_reg32(uint8_t reg_code)
{
    char *opcode_bytes = malloc(1); // 1-byte opcode
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x40 + reg_code; // Opcode for 'inc reg32'

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

void dec_reg32(uint8_t reg_code)
{
    char *opcode_bytes = malloc(1); // 1-byte opcode
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x48 + reg_code; // Opcode for 'dec reg32'

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

void neg(uint8_t reg_code)
{
    char *opcode_bytes = malloc(2); // 1-byte opcode + 1-byte ModR/M
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF7;            // Opcode for 'neg r/m32'
    opcode_bytes[1] = 0xD8 + reg_code; // ModR/M byte for 'r/m32'

    OpCode new_opcode;
    new_opcode.size = 2; // Total instruction size
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
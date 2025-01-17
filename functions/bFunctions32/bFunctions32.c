#include "bFunctions32.h"
#include "../functions.h"

/*
i-immediate
r-register
m-memory

functions:

mov32 r, i  :D
mov32 r, m  :D
mov32 r, mi :D
mov32 r, mr :D
mov32 r, r  :D

mov32 m, i  :D
mov32 m, r  :D

mov32 mi, i :D
mov32 mi, r :D

mov32 mr, i
mov32 mr, r
*/

// Function to move immediate value into a 32-bit register
void mov32_r_i(uint8_t reg_code, uint32_t value)
{
    char *opcode_bytes = malloc(5); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xB8 + reg_code; // Opcode for 'mov reg32, imm32'

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

void mov32_r_m(uint8_t reg, uint8_t mem_reg)
{
    char *opcode_bytes = malloc(2);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x8B;
    opcode_bytes[1] = 0x00 + (reg * 8) + mem_reg;

    OpCode new_opcode;
    new_opcode.size = 2;
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

void mov32_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x8B;                             // Opcode for MOV reg32, r/m32
    opcode_bytes[1] = 0x80 + reg_base + (reg_dest * 8); // Constructed ModR/M byte

    memcpy(&opcode_bytes[2], &offset, 4);

    OpCode new_opcode;
    new_opcode.size = 6;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov32_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
    char *opcode_bytes = malloc(3);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x8B;
    opcode_bytes[1] = 0x04 + (reg * 8);
    opcode_bytes[2] = (reg_base * 8) + reg_offset;

    OpCode new_opcode;
    new_opcode.size = 3;
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

void mov32_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x8B; // talvez 0x89
    opcode_bytes[1] = 0xC0 + reg1 + (reg2 * 8);

    OpCode new_opcode;
    new_opcode.size = 2;
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

void mov32_m_i(uint8_t reg1, uint32_t value)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xC7;
    opcode_bytes[1] = 0x00 + reg1;

    memcpy(&opcode_bytes[2], &value, 4);

    OpCode new_opcode;
    new_opcode.size = 6;
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

void mov32_m_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x00 + reg1 + (reg2 * 8);

    OpCode new_opcode;
    new_opcode.size = 2;
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

void mov32_mi_i(uint8_t reg, int32_t offset, uint32_t value)
{
    char *opcode_bytes = malloc(10);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xC7;
    opcode_bytes[1] = 0x80 + reg;
    memcpy(&opcode_bytes[2], &offset, 4);
    memcpy(&opcode_bytes[6], &value, 4);

    OpCode new_opcode;
    new_opcode.size = 10;
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

void mov32_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x80 + reg + (reg2 * 8);
    memcpy(&opcode_bytes[2], &offset, 4);

    OpCode new_opcode;
    new_opcode.size = 6;
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

void mov32_mr_i(uint8_t reg, uint8_t reg2, uint32_t value)
{
    char *opcode_bytes = malloc(7);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xC7;
    opcode_bytes[1] = 0x04;
    opcode_bytes[2] = (reg * 8) + reg2;

    memcpy(&opcode_bytes[3], &value, 4);

    OpCode new_opcode;
    new_opcode.size = 7;
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

void mov32_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
    char *opcode_bytes = malloc(3);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x04 + (reg3 * 8);
    opcode_bytes[2] = (reg_base * 8) + reg2;

    OpCode new_opcode;
    new_opcode.size = 3;
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

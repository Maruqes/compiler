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

// Function to move immediate value into a 32-bit register
void mov16_r_i(uint8_t reg_code, uint16_t value)
{
    char *opcode_bytes = malloc(4); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0xB8 + reg_code;

    memcpy(&opcode_bytes[2], &value, sizeof(value)); // Copy the 4-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 4; // Total instruction size
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

void mov16_r_m(uint8_t reg, uint8_t mem_reg)
{
    char *opcode_bytes = malloc(3);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x8B;
    opcode_bytes[2] = 0x00 + (reg * 8) + mem_reg;

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

void mov16_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
    char *opcode_bytes = malloc(7);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66;                             // Opcode for MOV reg32, r/m32
    opcode_bytes[1] = 0x8B;                             // Opcode for MOV reg32, r/m32
    opcode_bytes[2] = 0x80 + reg_base + (reg_dest * 8); // Constructed ModR/M byte

    memcpy(&opcode_bytes[3], &offset, 4);

    OpCode new_opcode;
    new_opcode.size = 7;
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

void mov16_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x8B;
    opcode_bytes[2] = 0x04 + (reg * 8);
    opcode_bytes[3] = (reg_base * 8) + reg_offset;

    OpCode new_opcode;
    new_opcode.size = 4;
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

void mov16_r_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x89;
    opcode_bytes[2] = 0xC0 + reg1 + (reg2 * 8);

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

void mov16_m_i(uint8_t reg1, uint16_t value)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0xC7;
    opcode_bytes[2] = 0x00 + reg1;

    memcpy(&opcode_bytes[3], &value, 2);

    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov16_m_r(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(3);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x89;
    opcode_bytes[2] = 0x00 + reg1 + (reg2 * 8);

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

void mov16_mi_i(uint8_t reg, int32_t offset, uint16_t value)
{
    char *opcode_bytes = malloc(9);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0xC7;
    opcode_bytes[2] = 0x80 + reg;
    memcpy(&opcode_bytes[3], &offset, 4);
    memcpy(&opcode_bytes[7], &value, 2);

    OpCode new_opcode;
    new_opcode.size = 9;
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

void mov16_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    char *opcode_bytes = malloc(7);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x89;
    opcode_bytes[2] = 0x80 + reg + (reg2 * 8);
    memcpy(&opcode_bytes[3], &offset, 4);

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

void mov16_mr_i(uint8_t reg, uint8_t reg2, uint16_t value)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0xC7;
    opcode_bytes[2] = 0x04;
    opcode_bytes[3] = (reg * 8) + reg2;

    memcpy(&opcode_bytes[4], &value, 2);

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

void mov16_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x66;
    opcode_bytes[1] = 0x89;
    opcode_bytes[2] = 0x04 + (reg3 * 8);
    opcode_bytes[3] = (reg_base * 8) + reg2;

    OpCode new_opcode;
    new_opcode.size = 4;
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

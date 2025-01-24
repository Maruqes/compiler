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
    uint8_t *opcode_bytes;
    size_t size;

    if (mem_reg == 4)
    {
        size = 3;
        opcode_bytes = malloc(size);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0] = 0x8B;
        opcode_bytes[1] = 0x04 + (reg * 8);
        opcode_bytes[2] = 0x24;
    }
    else if (mem_reg == 5)
    {
        size = 3;
        opcode_bytes = malloc(size);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0] = 0x8B;
        opcode_bytes[1] = 0x45 + (reg * 8);
        opcode_bytes[2] = 0x00;
    }
    else // Caso normal, sem ESP/EBP
    {
        size = 2; // opcode + ModR/M
        opcode_bytes = malloc(size);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0] = 0x8B;                            // MOV r32, r/m32
        opcode_bytes[1] = (0 << 6) | (reg << 3) | mem_reg; // mod=00, reg, r/m=mem_reg
    }

    OpCode new_opcode;
    new_opcode.size = size;
    new_opcode.code = (char *)opcode_bytes;

    // Adiciona ao array global de opcodes
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
    OpCode new_opcode;

    if (reg_base == REG_ESP)
    {
        char *opcode_bytes = malloc(7);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0] = 0x8B;                  // Opcode for MOV reg32, r/m32
        opcode_bytes[1] = 0x84 + (reg_dest * 8); // Constructed ModR/M byte
        opcode_bytes[2] = 0x24;                  // SIB byte

        memcpy(&opcode_bytes[3], &offset, 4);

        new_opcode.size = 7;
        new_opcode.code = opcode_bytes;
    }
    else if (reg_base == REG_EBP)
    {
        char *opcode_bytes = malloc(6);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0] = 0x8B;                  // Opcode for MOV reg32, r/m32
        opcode_bytes[1] = 0x85 + (reg_dest * 8); // Constructed ModR/M byte

        memcpy(&opcode_bytes[2], &offset, 4);

        new_opcode.size = 6;
        new_opcode.code = opcode_bytes;
    }
    else
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

        new_opcode.size = 6;
        new_opcode.code = opcode_bytes;
    }

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
    OpCode new_opcode;

    if (reg_base == REG_ESP)
    {
        char *opcode_bytes = malloc(3);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0] = 0x8B;
        opcode_bytes[1] = 0x04 + (reg * 8);
        opcode_bytes[2] = 0x04 + (reg_offset * 8);

        new_opcode.size = 3;
        new_opcode.code = opcode_bytes;
    }
    else if (reg_base == REG_EBP)
    {
        char *opcode_bytes = malloc(4);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0] = 0x8B;
        opcode_bytes[1] = 0x44 + (reg * 8);
        opcode_bytes[2] = 0x05 + (reg_offset * 8);
        opcode_bytes[3] = 0x00;

        new_opcode.size = 4;
        new_opcode.code = opcode_bytes;
    }
    else
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

        new_opcode.size = 3;
        new_opcode.code = opcode_bytes;
    }

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
    opcode_bytes[0] = 0x89;
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
    uint8_t *opcode_bytes;
    size_t size;

    if (reg1 == 4)
    {
        size = 7;
        opcode_bytes = malloc(size);

        opcode_bytes[0] = 0xC7;
        opcode_bytes[1] = 0x04;
        opcode_bytes[2] = 0x24;

        // Copiar os 4 bytes do immediate
        memcpy(&opcode_bytes[3], &value, 4);
    }
    else if (reg1 == 5)
    {
        size = 7;
        opcode_bytes = malloc(size);

        opcode_bytes[0] = 0xC7;
        opcode_bytes[1] = 0x45;
        opcode_bytes[2] = 0x00;

        memcpy(&opcode_bytes[3], &value, 4);
    }
    else
    {
        size = 6;
        opcode_bytes = malloc(size);

        opcode_bytes[0] = 0xC7;
        opcode_bytes[1] = (0 << 3) | reg1;
        memcpy(&opcode_bytes[2], &value, 4);
    }

    OpCode new_opcode;
    new_opcode.size = size;
    new_opcode.code = (char *)opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void mov32_m_r(uint8_t reg1, uint8_t reg2)
{
    OpCode new_opcode;
    if (reg1 == REG_ESP)
    {
        uint8_t *opcode_bytes;
        opcode_bytes = malloc(3);

        opcode_bytes[0] = 0x89;

        opcode_bytes[1] = 0x04 + (reg2 * 8);

        opcode_bytes[2] = 0x24;

        new_opcode.size = 3;
        new_opcode.code = opcode_bytes;
    }
    else if (reg1 == REG_EBP)
    {
        uint8_t *opcode_bytes;
        opcode_bytes = malloc(3);

        opcode_bytes[0] = 0x89;

        opcode_bytes[1] = 0x45 + (reg2 * 8);

        opcode_bytes[2] = 0x00;

        new_opcode.size = 3;
        new_opcode.code = opcode_bytes;
    }
    else
    {
        char *opcode_bytes = malloc(2);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0] = 0x89;
        opcode_bytes[1] = 0x00 + reg1 + (reg2 * 8);

        new_opcode.size = 2;
        new_opcode.code = opcode_bytes;
    }

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
    OpCode new_opcode;

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

// works for all :D
void mov32_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    OpCode new_opcode;

    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x80 + reg + (reg2 * 8);
    memcpy(&opcode_bytes[2], &offset, 4);

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

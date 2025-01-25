#include "bFunctions32.h"
#include "../functions.h"
#include "../../push_pop/push_pop.h"
#include "../../arithmetic/arithmetic.h"

// f3 0f 10 04 24  -> movss xmm0, [esp]
void mov_xmm0_esp(uint8_t xmmReg)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x10;
    opcode_bytes[3] = 0x04;
    opcode_bytes[4] = 0x24;

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

// f3 0f 11 04 24  -> movss [esp], xmm0
void mov_esp_xmm0(uint8_t xmmReg)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x11;
    opcode_bytes[3] = 0x04 + (xmmReg * 8);
    opcode_bytes[4] = 0x24;

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

void mov_x_r(uint8_t xmmReg, uint8_t reg)
{
    push_reg(reg);
    mov_xmm0_esp(xmmReg);
    add(REG_ESP, 4);
}

void mov_r_x(uint8_t xmmReg, uint8_t reg)
{
    sub(REG_ESP, 4);
    mov_esp_xmm0(xmmReg);
    pop_reg(reg);
}

// cvtsi2ss xmm, reg
void cvtsi2ss(uint8_t xmmReg, uint8_t reg)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x2A;
    opcode_bytes[3] = 0xC0 + (xmmReg * 8) + reg;

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

// cvttss2si reg, xmm   -> converte float (xmmX) para inteiro 32 bits, truncando.
void cvttss2si(uint8_t reg, uint8_t xmmReg)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x2C;
    opcode_bytes[3] = 0xC0 + (reg * 8) + xmmReg;

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

// cvtsi2ss xmm, reg   ->converte float (xmmX) para inteiro 32 bits, arredondando segundo o modo de arredondamento actual do SSE.
void cvtss2si(uint8_t reg, uint8_t xmmReg)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x2D;
    opcode_bytes[3] = 0xC0 + (reg * 8) + xmmReg;

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

void ucomiss(uint8_t xmm1, uint8_t xmm2)
{
    char *opcode_bytes = malloc(3);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x0F;
    opcode_bytes[1] = 0x2E;
    opcode_bytes[2] = 0xC0 + (xmm1 * 8) + xmm2;

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
// from down here not tested
void addss(uint8_t xmm1, uint8_t xmm2)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x58;
    opcode_bytes[3] = 0xC0 + (xmm1 * 8) + xmm2;

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

void subss(uint8_t xmm1, uint8_t xmm2)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x5C;
    opcode_bytes[3] = 0xC0 + (xmm1 * 8) + xmm2;

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

void mulss(uint8_t xmm1, uint8_t xmm2)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x59;
    opcode_bytes[3] = 0xC0 + (xmm1 * 8) + xmm2;

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

void divss(uint8_t xmm1, uint8_t xmm2)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x5E;
    opcode_bytes[3] = 0xC0 + (xmm1 * 8) + xmm2;

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


void sqrtss(uint8_t xmm1, uint8_t xmm2)
{
    char *opcode_bytes = malloc(4);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xF3;
    opcode_bytes[1] = 0x0F;
    opcode_bytes[2] = 0x51;
    opcode_bytes[3] = 0xC0 + (xmm1 * 8) + xmm2;

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

#include "variables.h"
#include "../functions/functions.h"
#include "../push_pop/push_pop.h"
#include "../mov_reg_reg/mov_reg_reg.h"
#include "../arithmetic/arithmetic.h"

Variable *variables_array = NULL;
uint32_t variables_array_size = 0;
uint64_t variables_size = 0;

void add_var_to_array(char *symbol, uint32_t size)
{
    Variable new_var;
    new_var.symbol = symbol;
    new_var.size = size;
    new_var.offset = variables_size + size;

    variables_array = realloc(variables_array,
                              (variables_array_size + 1) * sizeof(Variable));
    if (!variables_array)
    {
        perror("Failed to reallocate memory for variables_array");
        exit(EXIT_FAILURE);
    }
    variables_array[variables_array_size++] = new_var;

    variables_size += size;
}

// mov [reg_base + offset], reg2
void mov_reg_offset_reg2(uint8_t reg, uint32_t offset, uint8_t reg2)
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

// mov [reg_base + offset], value
void mov_reg_offset_value(uint8_t reg, int32_t offset, uint32_t value)
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

// mov reg, [reg_base + offset]
void mov_reg_reg_offset(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
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

void create_var(char *symbol, uint32_t size)
{
    add_var_to_array(symbol, size);

    sub(REG_ESP, size);
}

void set_var(char *symbol, uint32_t value)
{
    for (uint32_t i = 0; i < variables_array_size; i++)
    {
        if (strcmp(symbol, variables_array[i].symbol) == 0)
        {
            mov_reg_offset_value(REG_EBP, -variables_array[i].offset, value);
            return;
        }
    }

    fprintf(stderr, "Error: Symbol %s not found\n", symbol);
    exit(EXIT_FAILURE);
}

void set_var_with_reg(char *symbol, uint8_t reg)
{
    for (uint32_t i = 0; i < variables_array_size; i++)
    {
        if (strcmp(symbol, variables_array[i].symbol) == 0)
        {
            mov_reg_offset_reg2(REG_EBP, -variables_array[i].offset, reg);
            return;
        }
    }

    fprintf(stderr, "Error: Symbol %s not found\n", symbol);
    exit(EXIT_FAILURE);
}

void get_var(uint8_t reg, char *symbol)
{
    for (uint32_t i = 0; i < variables_array_size; i++)
    {
        if (strcmp(symbol, variables_array[i].symbol) == 0)
        {
            mov_reg_reg_offset(reg, REG_EBP, -variables_array[i].offset);
            return;
        }
    }

    fprintf(stderr, "Error: Symbol %s not found\n", symbol);
    exit(EXIT_FAILURE);
}

void create_new_stack()
{
    push_reg(REG_EBP);
    mov_reg32_reg32(REG_EBP, REG_ESP);
}

void restore_stack()
{
    mov_reg32_reg32(REG_ESP, REG_EBP);
    pop_reg(REG_EBP);
}

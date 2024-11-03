#include "variables.h"
#include "../functions/functions.h"
#include "../push_pop/push_pop.h"
#include "../mov_reg_reg/mov_reg_reg.h"
#include "../arithmetic/arithmetic.h"
#include "../parser/parser_help.h"

Variable *variables_array = NULL;
uint32_t variables_array_size = 0;
uint64_t variables_size = 0;

void add_var_to_array(char *symbol, uint32_t size, char *scope)
{
    Variable new_var;
    new_var.symbol = malloc(strlen(symbol) + 1);
    if (!new_var.symbol)
    {
        perror("Failed to allocate memory for new_var.symbol");
        exit(EXIT_FAILURE);
    }
    strcpy(new_var.symbol, symbol);
    new_var.size = size;
    new_var.offset = variables_size + size;

    if (scope)
    {
        new_var.scope = malloc(strlen(scope) + 1);
        if (!new_var.scope)
        {
            perror("Failed to allocate memory for new_var.scope");
            exit(EXIT_FAILURE);
        }
        strcpy(new_var.scope, scope);
    }
    else
    {
        new_var.scope = NULL;
    }

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

void create_var(char *symbol, uint32_t size)
{
    // check if var already exists
    if (does_var_exist(symbol))
    {
        fprintf(stderr, "Error: Symbol %s already exists\n", symbol);
        exit(EXIT_FAILURE);
    }

    add_var_to_array(symbol, size, get_current_scope());

    sub(REG_ESP, size);
}

void set_var(char *symbol, uint32_t value)
{
    for (uint32_t i = 0; i < variables_array_size; i++)
    {
        if (strcmp(symbol, variables_array[i].symbol) == 0 && strcmp(variables_array[i].scope, get_current_scope()) == 0)
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
        if (strcmp(symbol, variables_array[i].symbol) == 0 &&
            strcmp(variables_array[i].scope, get_current_scope()) == 0)
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
        if (strcmp(symbol, variables_array[i].symbol) == 0 &&
            strcmp(variables_array[i].scope, get_current_scope()) == 0)
        {
            mov_reg_reg_offset(reg, REG_EBP, -variables_array[i].offset);
            return;
        }
    }

    fprintf(stderr, "Error: Symbol %s not found\n", symbol);
    exit(EXIT_FAILURE);
}

int does_var_exist(char *symbol)
{
    for (uint32_t i = 0; i < variables_array_size; i++)
    {
        if (strcmp(symbol, variables_array[i].symbol) == 0 &&
            strcmp(variables_array[i].scope, get_current_scope()) == 0)
        {
            return 1;
        }
    }

    return 0;
}

Variable return_var_struct(char *symbol)
{
    for (uint32_t i = 0; i < variables_array_size; i++)
    {
        if (strcmp(symbol, variables_array[i].symbol) == 0 &&
            strcmp(variables_array[i].scope, get_current_scope()) == 0)
        {
            return variables_array[i];
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

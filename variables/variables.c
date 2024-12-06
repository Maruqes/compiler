#include "variables.h"
#include "../functions/functions.h"
#include "../push_pop/push_pop.h"
#include "../mov_reg_reg/mov_reg_reg.h"
#include "../arithmetic/arithmetic.h"
#include "../parser/parser_help.h"

typedef struct Scope_var
{
    Variable *variables_array;
    uint32_t variables_array_size;
    uint64_t variables_size;
    char *scope;
} Scope_var;

Scope_var *scopes_array;
uint32_t scopes_array_size = 0;

void create_new_scope_var(char *scope)
{
    // Check if the scope already exists
    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        if (strcmp(scope, scopes_array[i].scope) == 0)
        {
            printf("Scope %s already exists probably function with same name\n", scope);
            exit(1);
            return;
        }
    }

    // Create a new scope
    Scope_var new_scope;
    new_scope.scope = malloc(strlen(scope) + 1);
    if (!new_scope.scope)
    {
        perror("Failed to allocate memory for new_scope.scope");
        exit(EXIT_FAILURE);
    }
    strcpy(new_scope.scope, scope);
    new_scope.variables_array = NULL;
    new_scope.variables_array_size = 0;
    new_scope.variables_size = 0;

    scopes_array = realloc(scopes_array,
                           (scopes_array_size + 1) * sizeof(Scope_var));
    if (!scopes_array)
    {
        perror("Failed to reallocate memory for scopes_array");
        exit(EXIT_FAILURE);
    }
    scopes_array[scopes_array_size++] = new_scope;
}

Scope_var *get_scope_var(char *scope)
{
    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        if (strcmp(scope, scopes_array[i].scope) == 0)
        {
            return &scopes_array[i];
        }
    }

    fprintf(stderr, "Error: Scope %s not found\n", scope);
    exit(EXIT_FAILURE);
}

void add_var_to_array(char *symbol, uint32_t size, char *scope)
{
    Scope_var *current_scope = get_scope_var(scope);

    Variable new_var;
    new_var.symbol = malloc(strlen(symbol) + 1);
    if (!new_var.symbol)
    {
        perror("Failed to allocate memory for new_var.symbol");
        exit(EXIT_FAILURE);
    }
    strcpy(new_var.symbol, symbol);
    new_var.size = size;
    new_var.offset = current_scope->variables_size + size;

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

    current_scope->variables_array = realloc(current_scope->variables_array,
                                             (current_scope->variables_array_size + 1) * sizeof(Variable));
    if (!current_scope->variables_array)
    {
        perror("Failed to reallocate memory for variables_array");
        exit(EXIT_FAILURE);
    }
    current_scope->variables_array[current_scope->variables_array_size++] = new_var;

    current_scope->variables_size += size;
}

void free_variables_array()
{
    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        for (uint32_t j = 0; j < scopes_array[i].variables_array_size; j++)
        {
            free(scopes_array[i].variables_array[j].symbol);
            if (scopes_array[i].variables_array[j].scope)
            {
                free(scopes_array[i].variables_array[j].scope);
            }
        }
        free(scopes_array[i].variables_array);
        free(scopes_array[i].scope);
    }
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

    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        for (uint32_t j = 0; j < scopes_array[i].variables_array_size; j++)
        {
            if (strcmp(symbol, scopes_array[i].variables_array[j].symbol) == 0 && strcmp(scopes_array[i].variables_array[j].scope, get_current_scope()) == 0)
            {
                mov_reg_offset_value(REG_EBP, -scopes_array[i].variables_array[j].offset, value);
                return;
            }
        }
    }

    fprintf(stderr, "Error: Symbol %s not found\n", symbol);
    exit(EXIT_FAILURE);
}

void set_var_with_reg(char *symbol, uint8_t reg)
{
    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        for (uint32_t j = 0; j < scopes_array[i].variables_array_size; j++)
        {
            if (strcmp(symbol, scopes_array[i].variables_array[j].symbol) == 0 && strcmp(scopes_array[i].variables_array[j].scope, get_current_scope()) == 0)
            {
                mov_reg_offset_reg2(REG_EBP, -scopes_array[i].variables_array[j].offset, reg);
                return;
            }
        }
    }

    fprintf(stderr, "Error: Symbol %s not found\n", symbol);
    exit(EXIT_FAILURE);
}

void get_var(uint8_t reg, char *symbol)
{

    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        for (uint32_t j = 0; j < scopes_array[i].variables_array_size; j++)
        {
            if (strcmp(symbol, scopes_array[i].variables_array[j].symbol) == 0 && strcmp(scopes_array[i].variables_array[j].scope, get_current_scope()) == 0)
            {
                mov_reg_reg_offset(reg, REG_EBP, -scopes_array[i].variables_array[j].offset);
                return;
            }
        }
    }

    fprintf(stderr, "Error: Symbol %s not found\n", symbol);
    exit(EXIT_FAILURE);
}

int does_var_exist(char *symbol)
{
    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        for (uint32_t j = 0; j < scopes_array[i].variables_array_size; j++)
        {
            if (strcmp(symbol, scopes_array[i].variables_array[j].symbol) == 0 &&
                strcmp(scopes_array[i].variables_array[j].scope, get_current_scope()) == 0)
            {
                return 1;
            }
        }
    }

    return 0;
}

Variable return_var_struct(char *symbol)
{
    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        for (uint32_t j = 0; j < scopes_array[i].variables_array_size; j++)
        {
            if (strcmp(symbol, scopes_array[i].variables_array[j].symbol) == 0 &&
                strcmp(scopes_array[i].variables_array[j].scope, get_current_scope()) == 0)
            {
                return scopes_array[i].variables_array[j];
            }
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

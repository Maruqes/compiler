#include "variables.h"
#include "../functions/functions.h"
#include "../push_pop/push_pop.h"
#include "../mov_reg_reg/mov_reg_reg.h"
#include "../arithmetic/arithmetic.h"
#include "../parser/parser_help.h"
#include "../functions/bFunctions32/bFunctions32.h"
#include "../functions/bFunctions8/bFunctions8.h"
#include "../functions/bFunctions16/bFunctions16.h"
#include "../parser/int/parser_int.h"

/*
the stack goes down so

0xFFFFFFF
...
0x0000000


when we push we go down
when we pop we go up

when we create a new variable we substract from the stack pointer
when we delete a variable we add to the stack pointer

(we push old base to restore it later)
when we create a new stack we set base pointer to stack pointer
when we restore the stack we set stack pointer to base pointer

so access old vars its just ebp - offset
our funcs get/set already take the negative (-)

so for us programming here we set the values in the other order see func call "add_var_to_array_with_offset" in "get_params"
we set tha value negative in relation of ebp becouse we now our functions will negate it again making it positive
becouse params are on the old stack we should do [ebp + offset] to access them     and whe are doing [ebp -- offset] to set them the same :D
*/

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

void add_var_to_array_with_offset(char *symbol, uint32_t size, char *scope, uint32_t original_size, uint32_t offset, uint32_t type)
{
    printf("Adding var %s to array with offset %d\n", symbol, offset);
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
    new_var.original_size = original_size;
    new_var.offset = offset;
    new_var.type = type;

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
}

void add_var_to_array(char *symbol, uint32_t size, char *scope, uint32_t original_size, uint32_t type)
{
    Scope_var *current_scope = get_scope_var(scope);
    add_var_to_array_with_offset(symbol, size, scope, original_size, current_scope->variables_size + size, type);
    current_scope->variables_size += size;
}

// ANSI Color Codes
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"

void print_scopes_info()
{
    printf(BOLD CYAN "\n=== Scopes Information ===\n" RESET);

    for (uint32_t i = 0; i < scopes_array_size; i++)
    {
        printf(BOLD BLUE "\nScope [%u]: %s\n" RESET, i + 1, scopes_array[i].scope);
        printf(YELLOW "-------------------------------------------------------------\n" RESET);
        printf(GREEN "  %-15s | %-6s | %-14s | %-6s\n" RESET, "Variable", "Size", "Original Size", "Offset");
        printf(YELLOW "-------------------------------------------------------------\n" RESET);

        for (uint32_t j = 0; j < scopes_array[i].variables_array_size; j++)
        {
            printf("  %-15s | %-6u | %-14u | %-6u\n",
                   scopes_array[i].variables_array[j].symbol,
                   scopes_array[i].variables_array[j].size,
                   scopes_array[i].variables_array[j].original_size,
                   scopes_array[i].variables_array[j].offset);
        }
        printf(YELLOW "-------------------------------------------------------------\n" RESET);
    }
}

void free_variables_array()
{
    print_scopes_info();
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

void create_var(char *symbol, uint32_t size, uint32_t original_size, uint32_t type)
{
    // check if var already exists
    if (does_var_exist(symbol))
    {
        fprintf(stderr, "Error: Symbol %s already exists\n", symbol);
        exit(EXIT_FAILURE);
    }

    add_var_to_array(symbol, size, get_current_scope(), original_size, type);

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
                if (scopes_array[i].variables_array[j].size == get_type_length(DD))
                {
                    mov32_mi_i(REG_EBP, -scopes_array[i].variables_array[j].offset, value);
                }
                else if (scopes_array[i].variables_array[j].size == get_type_length(DW))
                {
                    mov16_mi_i(REG_EBP, -scopes_array[i].variables_array[j].offset, value);
                }
                else if (scopes_array[i].variables_array[j].size == get_type_length(DB))
                {
                    mov8_mi_i(REG_EBP, -scopes_array[i].variables_array[j].offset, value);
                }
                else
                {
                    printf("size %d\n", scopes_array[i].variables_array[j].size);
                    fprintf(stderr, "Error: Invalid size for symbol %s\n", symbol);
                    exit(EXIT_FAILURE);
                }
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
                if (scopes_array[i].variables_array[j].size == get_type_length(DD))
                {
                    mov32_mi_r(REG_EBP, -scopes_array[i].variables_array[j].offset, reg);
                }
                else if (scopes_array[i].variables_array[j].size == get_type_length(DW))
                {
                    mov16_mi_r(REG_EBP, -scopes_array[i].variables_array[j].offset, reg);
                }
                else if (scopes_array[i].variables_array[j].size == get_type_length(DB))
                {
                    mov8_mi_r(REG_EBP, -scopes_array[i].variables_array[j].offset, reg);
                }
                else
                {
                    printf("size %d\n", scopes_array[i].variables_array[j].size);
                    fprintf(stderr, "Error: Invalid size for symbol %s\n", symbol);
                    exit(EXIT_FAILURE);
                }
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
                if (scopes_array[i].variables_array[j].size == get_type_length(DD))
                {
                    mov32_r_mi(reg, REG_EBP, -scopes_array[i].variables_array[j].offset);
                }
                else if (scopes_array[i].variables_array[j].size == get_type_length(DW))
                {
                    mov32_r_i(reg, 0); // clear reg
                    mov16_r_mi(reg, REG_EBP, -scopes_array[i].variables_array[j].offset);
                }
                else if (scopes_array[i].variables_array[j].size == get_type_length(DB))
                {
                    mov32_r_i(reg, 0); // clear reg
                    mov8_r_mi(reg, REG_EBP, -scopes_array[i].variables_array[j].offset);
                }
                else
                {
                    printf("size %d\n", scopes_array[i].variables_array[j].size);
                    fprintf(stderr, "Error: Invalid size for symbol %s\n", symbol);
                    exit(EXIT_FAILURE);
                }
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

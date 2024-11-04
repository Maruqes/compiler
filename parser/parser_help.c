#include "parser_help.h"
#include <ctype.h>
#include <math.h>
#include "../types/uint32.h"
#include "../functions/functions.h"
#include "../types/strings.h"
#include "../mov_reg_reg/mov_reg_reg.h"
#include "../push_pop/push_pop.h"
#include "../16_code/functions_16.h"
#include "../8low_code/functions_8low.h"
#include "../8high_code/functions_8high.h"
#include "../jumps/jumps.h"
#include "../arithmetic/arithmetic.h"
#include "../logic/logic.h"
#include "../variables/variables.h"
#include "parser.h"
#include "int/parser_int.h"

Function_struct *functions;
uint32_t functions_count = 0;

int is_a_function(char *token)
{
    for (int i = 0; i < functions_count; i++)
    {
        if (strcmp(functions[i].name, token) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void add_function(char *name, char *return_type)
{
    printf("Adding function %s\n", name);
    functions = realloc(functions, sizeof(Function_struct) * (functions_count + 1));
    if (functions == NULL)
    {
        printf("Error: Could not allocate memory for functions\n");
        exit(1);
    }
    functions[functions_count].name = malloc(strlen(name) + 1);
    strcpy(functions[functions_count].name, name);
    functions[functions_count].return_type = return_type;
    functions_count++;
}

int checkFuncType(char *type)
{
    if (strcmp(type, "int") != 0)
    {
        return 1;
    }
    return 0;
}

char *current_scope = NULL;

void set_current_scope(char *scope)
{
    if (current_scope != NULL)
    {
        free(current_scope);
    }
    current_scope = malloc(strlen(scope) + 1);
    strcpy(current_scope, scope);
}

char *get_current_scope()
{
    return current_scope;
}

// will get something like int *****a = &b; and return the value of *****b, go to the root of the pointers
void multiple_dereference(FILE *file, char *var, uint8_t reg)
{
    if (reg == REG_EDX)
    {
        printf("EDX can not be used here func: multiple_dereference\n");
        exit(1);
    }

    int number_of_dereferences = 0; // only the extra

    while (var[0] == '*')
    {
        var = get_token(file);
        number_of_dereferences++;
    }

    get_var(REG_ECX, var);
    for (int i = 0; i < number_of_dereferences; i++)
    {
        mov_reg_reg_with_offset(REG_EDX, REG_EBP, REG_ECX); // ebp + ecx = value da var, entao edx = value da var
                                                            // valor esse que tambem é um address, repetir o processo
        mov_reg32_reg32(REG_ECX, REG_EDX);
    }

    mov_reg_reg_with_offset(reg, REG_EBP, REG_ECX);
}

// will parse normal ints/poinetrs functions and vars
void parse_data_types(FILE *file, char *token, uint8_t reg)
{ // handle arithemetics
    /*
        tipos de data
            normal-20
            funcao- func return 20
            var- a = 20
            var pointer- a = &b
    */
    // return no eax  que esta depois do =
    // check tipo de data

    if (reg == REG_ECX)
    {
        printf("ECX can not be used here func: parse_data_types\n");
        exit(1);
    }

    if (does_var_exist(token))
    {
        get_var(reg, token);
    }
    else if (token[0] == '*')
    {
        char *var = get_token(file);
        multiple_dereference(file, var, reg);
        free(var);
    }
    else if (token[0] == '&')
    {
        char *var = get_token(file);

        Variable var_struct = return_var_struct(var);
        mov_reg32(reg, -var_struct.offset);
        free(var);
    }
    else if (token[0] == '\'')
    {
        char *var = get_token(file);
        char *p = get_token(file); // skip the '
        if (p[0] != '\'')
        {
            printf("Error: Expected '\n");
            exit(1);
        }

        mov_reg32(reg, var[0]);

        free(var);
        free(p);
    }
    else if (is_a_function(token))
    {
        push_eax();
        call(token);
        mov_reg32_reg32(reg, REG_EAX);
        pop_eax();
    }
    else if (is_a_uint32_beforeconstant(token))
    {
        mov_reg32_from_var(reg, token, 0);
    }
    else if (is_a_uint32_arr_beforeconstant(token))
    {
        if (reg == REG_ECX)
        {
            printf("ECX can not be used here func: parse_data_types\n");
            exit(1);
        }

        char *token2 = get_token(file); // skip [
        if (token2[0] != '[')
        {
            printf("Error: Expected '['\n");
            exit(1);
        }

        push_reg(REG_ECX);
        push_reg(REG_EDX);
        push_reg(REG_EAX);

        parse_inside_bracets_for_arrays(file); // get the index []
        mov_reg32_reg32(REG_ECX, REG_EAX);

        pop_reg(REG_EAX);

        mov_edx(4);
        mul_reg32(REG_ECX, REG_EDX); // ecx = ecx * 4  precisa porque o offset é em INT (4 bytes)
        // melhor explicado em func: "parse_string_array_value_setter" parset_help.c

        pop_reg(REG_EDX);

        mov_reg32_from_var_offREG(reg, token, REG_ECX);

        pop_reg(REG_ECX);

        free(token2);
    }
    else
    {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        {
            if (token[0] == '-')
            {
                uint32_t val = atoi(token);
                mov_reg32(reg, -val);
            }
            else
            {
                uint32_t val = atoi(token);
                mov_reg32(reg, val);
            }
        }
        else
        {
            printf("Error parse_data_types: Symbol '%s' not found\n", token);
            exit(1);
        }
    }
}

// returns in EAX the value of the expression
void parse_after_equal(FILE *file)
{
    char *next_token = get_token(file);
    if (next_token[0] == ';')
    {
        return;
    }

    parse_data_types(file, next_token, REG_EAX);

    next_token = get_token(file);
    while (next_token[0] != ';')
    {
        if (next_token[0] == '+')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);
            add_reg32(REG_EAX, REG_EBX);
            free(var);
        }
        else if (next_token[0] == '-')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);
            sub_reg32(REG_EAX, REG_EBX);
            free(var);
        }
        else if (next_token[0] == '/')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            div_reg32(REG_ECX, REG_EBX);
            mov_reg32_reg32(REG_EAX, REG_ECX);

            free(var);
        }
        else if (next_token[0] == '*')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            mul_reg32(REG_ECX, REG_EBX);
            mov_reg32_reg32(REG_EAX, REG_ECX);

            free(var);
        }
        else if (next_token[0] == '\n')
        {
            printf("Error: Expected ';'\n");
            exit(1);
        }
        else
        {
            printf("Error: Expected operator\n");
            exit(1);
        }
        next_token = get_token(file);
    }

    free(next_token);
}

// returns in EAX whats inside [ ]
void parse_inside_bracets_for_arrays(FILE *file)
{
    char *next_token = get_token(file);
    if (next_token[0] == ']')
    {
        printf("Error: Expected number inside []\n");
        exit(1);
    }

    parse_data_types(file, next_token, REG_EAX);

    next_token = get_token(file);
    while (next_token[0] != ']')
    {
        if (next_token[0] == '+')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);
            add_reg32(REG_EAX, REG_EBX);
            free(var);
        }
        else if (next_token[0] == '-')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);
            sub_reg32(REG_EAX, REG_EBX);
            free(var);
        }
        else if (next_token[0] == '/')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            div_reg32(REG_ECX, REG_EBX);
            mov_reg32_reg32(REG_EAX, REG_ECX);

            free(var);
        }
        else if (next_token[0] == '*')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            mul_reg32(REG_ECX, REG_EBX);
            mov_reg32_reg32(REG_EAX, REG_ECX);

            free(var);
        }
        else
        {
            printf("Error: Expected operator\n");
            exit(1);
        }
        next_token = get_token(file);
    }

    free(next_token);
}

// string functions
void parse_string_array_value_setter(FILE *file, char *arr_var_name)
{
    char *token = get_token(file); // get [
    if (token[0] != '[')
    {
        printf("Error: Expected '['\n");
        exit(1);
    }

    parse_inside_bracets_for_arrays(file); // get the index []
    mov_reg32_reg32(REG_ECX, REG_EAX);
    // seria multiplicado por 1 porque o offset de cada byte é 1
    // em caso de int é necessario multiplicar ecx por 4
    // porque ecx é o offset arr[1] em inteiro é (arr_addr + 4)

    get_token(file); // skip '='
    parse_after_equal(file);

    mov_var_from_reg8_offREG(REG_AL, arr_var_name, REG_ECX);

    free(token);
    free(arr_var_name);
}
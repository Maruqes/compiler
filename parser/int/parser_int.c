#include "parser_int.h"
#include <ctype.h>
#include <math.h>
#include "../../types/uint32.h"
#include "../../functions/functions.h"
#include "../../types/strings.h"
#include "../../mov_reg_reg/mov_reg_reg.h"
#include "../../push_pop/push_pop.h"
#include "../../16_code/functions_16.h"
#include "../../8low_code/functions_8low.h"
#include "../../8high_code/functions_8high.h"
#include "../../jumps/jumps.h"
#include "../../arithmetic/arithmetic.h"
#include "../../logic/logic.h"
#include "../../variables/variables.h"
#include "../parser.h"
#include "../parser_help.h"

uint32_t parse_next_values_arithemtic(FILE *file)
{
    char *next_token = get_token(file);

    while (next_token[0] != ';')
    {
        if (next_token[0] == '+')
        {
            char *var = get_token(file);
            if (does_var_exist(var))
            {
                get_var(REG_EBX, var);
                add_reg32(REG_EAX, REG_EBX);
            }
            else if (is_a_function(var))
            {
                push_eax();
                call(var);
                mov_reg32_reg32(REG_EBX, REG_EAX);
                pop_eax();
                add_reg32(REG_EAX, REG_EBX);
            }
            else
            {
                mov_ebx(atoi(var));
                add_reg32(REG_EAX, REG_EBX);
            }
        }
        else if (next_token[0] == '-')
        {
            char *var = get_token(file);
            if (does_var_exist(var))
            {
                get_var(REG_EBX, var);
                sub_reg32(REG_EAX, REG_EBX);
            }
            else if (is_a_function(var))
            {
                push_eax();
                call(var);
                mov_reg32_reg32(REG_EBX, REG_EAX);
                pop_eax();
                sub_reg32(REG_EAX, REG_EBX);
            }
            else
            {
                mov_ebx(atoi(var));
                sub_reg32(REG_EAX, REG_EBX);
            }
            free(var);
        }
        else if (next_token[0] == '/')
        {
            char *var = get_token(file);
            if (does_var_exist(var))
            {
                get_var(REG_EBX, var);
                div_reg32(REG_EAX, REG_EBX);
            }
            else if (is_a_function(var))
            {
                push_eax();
                call(var);
                mov_reg32_reg32(REG_EBX, REG_EAX);
                pop_eax();
                div_reg32(REG_EAX, REG_EBX);
            }
            else
            {
                mov_ebx(atoi(var));
                div_reg32(REG_EAX, REG_EBX);
            }
            free(var);
        }
        else if (next_token[0] == '*')
        {
            char *var = get_token(file);
            if (does_var_exist(var))
            {
                get_var(REG_EBX, var);
                mul_reg32(REG_EAX, REG_EBX);
            }
            else if (is_a_function(var))
            {
                push_eax();
                call(var);
                mov_reg32_reg32(REG_EBX, REG_EAX);
                pop_eax();
                mul_reg32(REG_EAX, REG_EBX);
            }
            else
            {
                mov_ebx(atoi(var));
                mul_reg32(REG_EAX, REG_EBX);
            }
            free(var);
        }

        next_token = get_token(file);
    }

    free(next_token);
}

// pointers int
void parse_create_int_pointer(FILE *file, char *token)
{
    char *name = get_token(file);
    get_token(file); // skip '='

    char *value = get_token(file);

    printf("int *%s = %s\n", name, value);

    if (does_var_exist(value))
    {

        printf("int %s = %s\n", name, value);
        create_var(name, 4);

        Variable var = return_var_struct(value);

        mov_eax(-var.offset);
        set_var_with_reg(name, REG_EAX);
    }

    free(name);
    free(value);
    free(token);
}

void parse_set_value_in_the_address(FILE *file, char *token)
{
    char *p = get_token(file); // skip '='
    char *value = get_token(file);

    printf("%s* = %s\n", token, value);

    // vamos assumir que value é um inteiro
    get_var(REG_EAX, token);
    mov_reg_with_regOffset_value(REG_EBP, REG_EAX, atoi(value)); // mov [ebp + offset(REG_EAX)], value

    free(value);
    free(token);
}

// normal ints
void parse_create_int(FILE *file, char *token)
{
    char *name = get_token(file);
    if (name[0] == '*')
    {
        parse_create_int_pointer(file, name);
        return;
    }
    get_token(file); // skip '='

    char *value = get_token(file);

    if (does_var_exist(value))
    {

        printf("int %s = %s\n", name, value);
        create_var(name, 4);

        get_var(REG_EAX, value);
        parse_next_values_arithemtic(file);
        set_var_with_reg(name, REG_EAX);
    }
    else if (is_a_function(value))
    {
        // functions calls return
        create_var(name, 4);

        printf("Calling function %s\n", value);
        call(value);
        parse_next_values_arithemtic(file);
        set_var_with_reg(name, REG_EAX);
    }
    else
    {
        printf("int %s = %s\n", name, value);
        create_var(name, 4);

        mov_eax(atoi(value));
        parse_next_values_arithemtic(file);
        set_var_with_reg(name, REG_EAX);
    }

    free(name);
    free(value);
    free(token);
}

void parse_int_setter(FILE *file, char *token)
{
    char *p = get_token(file); // skip '='
    if (p[0] == '*')
    {
        parse_set_value_in_the_address(file, token);
        return;
    }

    char *value = get_token(file);
    uint32_t val = 0;

    // var
    if (does_var_exist(value))
    {
        get_var(REG_EAX, value);
        parse_next_values_arithemtic(file);
        set_var_with_reg(token, REG_EAX);

        printf("int %s = %s\n", token, value);
    }
    else if (is_a_function(value))
    {
        // functions calls return
        printf("Calling function %s\n", value);
        call(value);
        parse_next_values_arithemtic(file);
        set_var_with_reg(token, REG_EAX);
    }
    else
    {
        val = atoi(value);
        mov_eax(val);

        parse_next_values_arithemtic(file);

        set_var_with_reg(token, REG_EAX);
        printf("int %s = %d\n", token, val);
    }

    free(value);
    free(token);
}
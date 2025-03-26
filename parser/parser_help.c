#include "parser_help.h"
#include <ctype.h>
#include <math.h>
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
#include "strings/strings.h"
#include "../functions/bFunctions32/bFunctions32.h"
#include "../functions/bFunctions8/bFunctions8.h"

// will get something like int *****a = &b; and return the value of *****b, go to the root of the pointers
// this should free var;
void multiple_dereference(FILE *file, char *var, uint8_t reg)
{
    if (reg == REG_EDX)
    {
        printf("EDX can not be used here func: multiple_dereference\n");
        exit(1);
    }

    int number_of_dereferences = 0; // only the extra
    // var = *
    while (var[0] == '*')
    {
        free(var);
        var = get_token(file);
        number_of_dereferences++;
    }

    // var = 'var'
    int type = return_var_struct(var).original_size; // see struct var comment

    // deadressing ate a ultimo valor
    get_var(REG_ECX, var);
    for (int i = 0; i < number_of_dereferences; i++)
    {
        // ebp + ecx = value da var, entao edx = value da var
        // valor esse que tambem é um address, repetir o processo
        mov32_16_r_mr(REG_EDX, REG_EBP, REG_ECX, 0);
        mov_reg32_reg32(REG_ECX, REG_EDX);
    }

    // get the value of the last address
    mov32_16_r_i(reg, 0, 0);
    if (get_type_length(type) == 4)
    {
        mov32_16_r_mr(reg, REG_EBP, REG_ECX, 0);
    }
    else if (get_type_length(type) == 2)
    {
        mov32_16_r_mr(reg, REG_EBP, REG_ECX, 1);
    }
    else if (get_type_length(type) == 1)
    {
        mov8_r_mr(reg, REG_EBP, REG_ECX);
    }
    free(var);
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

    if (reg == REG_EAX)
    {
        printf("EAX can not be used here func: parse_data_types\n");
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
        parse_functions(file, token);
        mov_reg32_reg32(reg, REG_EAX);
        pop_eax();
    }
    // inside functions
    else if (parse_inside_functions(file, token, reg) == 1)
    {
        return;
    }
    else
    {

        if (is_valid_number(token))
        {
            uint32_t val = atoi(token);
            mov_reg32(reg, val);
        }
        else if (token[0] == '-')
        {
            char *val = get_token(file);
            uint32_t val_int = atoi(val);
            mov_reg32(reg, -val_int);
            free(val);
        }
        else if (token[0] == 'x')
        {
            uint32_t val = parse_hexadecimal(token);
            mov_reg32(reg, val);
        }
        else
        {
            printf("Error parse_data_types: Symbol '%s' not found\n", token);
            exit(1);
        }
    }
}

int cmp_char_charset(char c, char *charset)
{
    for (int i = 0; i < strlen(charset); i++)
    {
        if (c == charset[i])
        {
            return 1;
        }
    }
    return 0;
}

// returns in EAX the value of the expression
char *parse_until_charset(FILE *file, char *charset)
{
    printf("Parsing until charset: %s\n", charset);
    char *next_token = get_token(file);
    if (cmp_char_charset(next_token[0], charset) == 1)
    {
        printf("End of parsing\n");
        return next_token;
    }
    parse_data_types(file, next_token, REG_EBX);
    mov_reg32_reg32(REG_EAX, REG_EBX);

    free(next_token);
    next_token = get_token(file);
    while (cmp_char_charset(next_token[0], charset) == 0)
    {
        // push_reg(REG_EBX);   tirei porque nao achei necessario TEM POP EM BAIXO
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
        else if (next_token[0] == '%')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            mod_reg32(REG_ECX, REG_EBX);
            mov_reg32_reg32(REG_EAX, REG_ECX);

            free(var);
        }
        else if (next_token[0] == '=')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            cmp_reg32(REG_ECX, REG_EBX);
            mov_reg32(REG_EAX, 0);
            sete_r(REG_EAX, 0);

            free(var);
        }
        else if (next_token[0] == '!')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            cmp_reg32(REG_ECX, REG_EBX);
            mov_reg32(REG_EAX, 0);
            setne_r(REG_EAX, 0);

            free(var);
        }
        else if (next_token[0] == '<')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            cmp_reg32(REG_ECX, REG_EBX);
            mov_reg32(REG_EAX, 0);
            setl_r(REG_EAX, 0);

            free(var);
        }
        else if (next_token[0] == '>')
        {
            char *var = get_token(file);
            parse_data_types(file, var, REG_EBX);

            mov_reg32_reg32(REG_ECX, REG_EAX);
            cmp_reg32(REG_ECX, REG_EBX);
            mov_reg32(REG_EAX, 0);
            setg_r(REG_EAX, 0);

            free(var);
        }
        else if (next_token[0] == '\n')
        {
            printf("Error: Expected ';'\n");
            exit(1);
        }
        else
        {
            printf("Next token: %s\n", next_token);
            printf("Error: Expected operator\n");
            exit(1);
        }
        // pop_reg(REG_EBX);
        free(next_token);
        next_token = get_token(file);
    }
    printf("End of parsing returning %s\n", next_token);
    return next_token;
}

// returns in EAX the value of the expression
void parse_after_equal(FILE *file)
{
    char *to = parse_until_charset(file, ";\n");
    printf("to: %s\n", to);
    free(to);
}

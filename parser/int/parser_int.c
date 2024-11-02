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

void parse_after_equal(FILE *file);
void parse_int_array_creation(FILE *file, char *token, uint32_t arr_size);

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

    printf("%s\n", token);

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
        char *token2 = get_token(file); // skip [
        if (token2[0] != '[')
        {
            printf("Error: Expected '['\n");
            exit(1);
        }
        char *index = get_token(file);
        token2 = get_token(file); // skip ]
        if (token2[0] != ']')
        {
            printf("Error: Expected ']'\n");
            exit(1);
        }
        mov_reg32_from_var(reg, token, atoi(index) * 4);
    }
    else
    {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1])))
        {
            uint32_t val = atoi(token);
            mov_reg32(reg, val);
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
        next_token = get_token(file);
    }

    free(next_token);
}

// pointers int
void parse_create_int_pointer(FILE *file, char *token)
{
    char *name = get_token(file);
    get_token(file); // skip '='

    create_var(name, 4);

    parse_after_equal(file);
    set_var_with_reg(name, REG_EAX);

    printf("int*%s\n", name);

    free(name);
    free(token);
}

void parse_set_value_in_the_pointer_address(FILE *file)
{
    int numbe_of_deferences = 0;
    char *var = get_token(file);
    while (var[0] == '*')
    {
        var = get_token(file);
        numbe_of_deferences++;
    }

    get_var(REG_ECX, var);
    for (int i = 0; i < numbe_of_deferences; i++)
    {
        mov_reg_reg_with_offset(REG_EDX, REG_EBP, REG_ECX); // ebp + ecx = value da var, entao edx = value da var
                                                            // valor esse que tambem é um address, repetir o processo
        mov_reg32_reg32(REG_ECX, REG_EDX);
    }

    char *p = get_token(file); // skip '='

    parse_after_equal(file);
    mov_reg_with_regOffset_reg(REG_EBP, REG_ECX, REG_EAX);

    free(var);
    free(p);
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
    else if (name[0] == '[')
    {
        char *size = get_token(file); // get the ']'
        char *p = get_token(file);    // get the ']'
        if (p[0] != ']')
        {
            printf("Error: Expected ']'\n");
            exit(1);
        }
        parse_int_array_creation(file, name, atoi(size));

        free(size);
        free(p);
        return;
    }
    get_token(file); // skip '='

    printf("int %s\n", name);
    create_var(name, 4);
    parse_after_equal(file);
    set_var_with_reg(name, REG_EAX);

    free(name);
    free(token);
}

void parse_int_setter(FILE *file, char *token)
{
    get_token(file); // skip '='

    parse_after_equal(file);
    set_var_with_reg(token, REG_EAX);

    free(token);
}

// array
void parse_int_array_value_setter(FILE *file, char *arr_var_name)
{
    printf("%s uihdn", arr_var_name);
    char *token = get_token(file);
    if (token[0] != '[')
    {
        printf("Error: Expected '['\n");
        exit(1);
    }

    char *index = get_token(file);

    token = get_token(file);
    if (token[0] != ']')
    {
        printf("Error: Expected ']'\n");
        exit(1);
    }

    get_token(file); // skip '='
    parse_after_equal(file);

    mov_var_from_reg32(REG_EAX, arr_var_name, atoi(index) * 4);

    free(index);
    free(token);
    free(arr_var_name);
}

void parse_int_array_creation(FILE *file, char *token, uint32_t arr_size)
{
    char *name = get_token(file);
    get_token(file); // skip '='

    printf("int[%d] %s\n", arr_size, name);

    char *val = get_token(file);
    if (val[0] != '{')
    {
        printf("Error: Expected '{'\n");
        exit(1);
    }

    uint32_t i = 0;
    val = get_token(file);

    uint32_t *arr = malloc(sizeof(uint32_t) * arr_size);

    while (val[0] != '}')
    {
        if (val[0] == ',')
        {
            val = get_token(file);
        }

        printf("int %s[%d] = %s\n", name, i, val);
        arr[i] = atoi(val);
        i++;
        val = get_token(file);
    }

    create_constant_uint32_arr_before(name, arr, arr_size);

    // dont free name because it will be used by constant_uint32_arr
    free(val);
    free(token);
}
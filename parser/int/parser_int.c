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

// pointers int
void parse_create_int_pointer(FILE *file, char *token)
{
    char *name = get_token(file);
    char *to_free = get_token(file); // skip '='
    free(to_free);

    if (does_var_exist(name))
    {
        printf("Error: Variable %s already exists\n", name);
        exit(1);
    }

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
void parse_create_int(FILE *file)
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
    char *to_free = get_token(file); // skip '='
    free(to_free);

    if (does_var_exist(name))
    {
        printf("Error: Variable %s already exists\n", name);
        exit(1);
    }

    printf("int %s\n", name);
    create_var(name, 4);
    parse_after_equal(file);
    set_var_with_reg(name, REG_EAX);

    free(name);
}

void parse_int_setter(FILE *file, char *token)
{
    char *to_free = get_token(file); // skip '='
    free(to_free);

    parse_after_equal(file);
    set_var_with_reg(token, REG_EAX);

    free(token);
}

// array
// arr[3] = 10;
void parse_int_array_value_setter(FILE *file, char *arr_var_name)
{
    char *token = get_token(file); // get [
    if (token[0] != '[')
    {
        printf("Error: Expected '['\n");
        exit(1);
    }

    push_reg(REG_ECX);
    parse_inside_bracets_for_arrays(file); // get the index []
    mov_reg32_reg32(REG_ECX, REG_EAX);

    mov_edx(4);
    mul_reg32(REG_ECX, REG_EDX);     // ecx = ecx * 4  precisa porque o offset é em INT (4 bytes)
                                     // melhor explicado em func: "parse_string_array_value_setter" parset_help.c
    char *to_free = get_token(file); // skip '='
    free(to_free);
    parse_after_equal(file);

    mov_var_from_reg32_offREG(REG_EAX, arr_var_name, REG_ECX);

    pop_reg(REG_ECX);

    free(token);
    free(arr_var_name);
}

void parse_int_array_creation(FILE *file, char *token, uint32_t arr_size)
{
    char *name = get_token(file);
    char *to_free = get_token(file); // skip '='
    free(to_free);

    printf("int[%u] %s\n", arr_size, name);

    char *val = get_token(file);
    if (val[0] != '{')
    {
        printf("Error: Expected '{'\n");
        exit(1);
    }

    uint32_t i = 0;

    free(val);
    val = get_token(file);

    uint32_t *arr = malloc(sizeof(uint32_t) * arr_size);

    while (val[0] != '}')
    {
        if (val[0] == ',')
        {
            free(val);
            val = get_token(file);
        }

        printf("int %s[%u] = %s\n", name, i, val);
        arr[i] = atoi(val);
        i++;
        free(val);
        val = get_token(file);
    }

    create_constant_uint32_arr_before(name, arr, arr_size);

    // dont free name because it will be used by constant_uint32_arr
    free(val);
    free(arr);
    free(token);
}
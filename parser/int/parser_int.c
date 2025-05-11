#include "parser_int.h"
#include <ctype.h>
#include <math.h>
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
#include "../../functions/bFunctions32/bFunctions32.h"

void parse_after_equal(FILE *file);
void parse_int_array_creation(FILE *file, char *token, uint32_t arr_size);

// pointers int
void parse_create_int_pointer(FILE *file, char *token, uint32_t original_size, uint32_t type)
{
    char *name = get_token(file);
    char *to_free = get_token(file); // skip '='
    free(to_free);

    if (does_var_exist(name))
    {
        printf("Error: Variable %s already exists\n", name);
        exit(1);
    }

    // dd becouse its lenght if 4
    create_var(name, get_type_length(DD), original_size, type);

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
        mov32_16_r_mi(REG_EDX, REG_ECX, 0, 0);
        mov_reg32_reg32(REG_ECX, REG_EDX);
    }

    char *p = get_token(file); // skip '='

    parse_after_equal(file);
    mov32_16_mi_r(REG_ECX, 0, REG_EAX, 0);

    free(var);
    free(p);
}

// normal ints
void parse_create_int(FILE *file, int type)
{
    char *name = get_token(file);
    if (name[0] == '*')
    {
        // 4 is the size of a pointer
        parse_create_int_pointer(file, name, get_type_length(type), type);
        return;
    }

    char *semi_or_equals = get_token(file); // skip '='

    if (does_var_exist(name))
    {
        printf("Error: Variable %s already exists\n", name);
        exit(1);
    }

    // check for empty name
    if (name[0] == '\n' || name[0] == '\0' || name[0] == ';' || name[0] == ' ')
    {
        printf("Error: Expected variable name\n");
        exit(1);
    }

    printf("int %s\n", name);
    printf("type %d\n", type);
    printf("size %d\n", get_type_length(type));
    create_var(name, get_type_length(type), get_type_length(type), type);

    if (semi_or_equals[0] == '=')
    {
        parse_after_equal(file);
        set_var_with_reg(name, REG_EAX);
    }
    else if (semi_or_equals[0] == ';')
    {
        set_var(name, 0);
    }
    else
    {
        printf("Error: Expected '=' or ';'\n");
        exit(1);
    }

    free(name);
    free(semi_or_equals);
}

void parse_int_setter(FILE *file, char *token)
{
    char *op_type = get_token(file); // skip
    if (op_type[0] != '=')
    {
        if (op_type[0] == '+')
        {
            char *op_type_next = get_token(file);
            get_check_free_semicolon(file);
            if (op_type_next[0] == '+')
            {
                get_var(REG_EAX, token);
                inc_reg32(REG_EAX);
                set_var_with_reg(token, REG_EAX);
                free(op_type);
                free(op_type_next);
                free(token);
                return;
            }
        }
        else if (op_type[0] == '-')
        {
            char *op_type_next = get_token(file);
            get_check_free_semicolon(file);
            if (op_type_next[0] == '-')
            {
                get_var(REG_EAX, token);
                dec_reg32(REG_EAX);
                set_var_with_reg(token, REG_EAX);
                free(op_type);
                free(op_type_next);
                free(token);
                return;
            }
        }
        printf("Error: Expected '=' '++' '--'\n");
        exit(1);
    }
    else if (op_type[0] == '=')
    {
        free(op_type);

        parse_after_equal(file);
        set_var_with_reg(token, REG_EAX);

        free(token);
    }
}

int check_type(char *type)
{
    if (strcmp(type, "db") == 0)
    {
        return DB;
    }
    else if (strcmp(type, "dw") == 0)
    {
        return DW;
    }
    else if (strcmp(type, "dd") == 0)
    {
        return DD;
    }
    return 0;
}

uint32_t get_type_length(int valueType)
{
    if (valueType == DB)
    {
        return DB;
    }
    else if (valueType == DW)
    {
        return DW;
    }
    else if (valueType == DD)
    {
        return DD;
    }
    else
    {
        printf("Error: Unknown type\n");
        exit(1);
    }
}
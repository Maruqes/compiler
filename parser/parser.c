#include "parser.h"
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

char *funcs_tokens[] = {"func", "endfunc", "return", "for", "endfor"};
char *vars_tokens[] = {"int"};
char *symbol_tokens[] = {";", "=", "<", "(", "{", "}", ")", ">", "=="};
char *arithmetic_symbols[] = {"+", "-", "*", "/", "^"};
char **token_save;

int parse_it(char *token, FILE *file);

int is_symbol(char token)
{
    for (int i = 0; i < sizeof(symbol_tokens) / sizeof(symbol_tokens[0]); i++)
    {
        if (token == symbol_tokens[i][0])
        {
            return 1;
        }
    }

    for (int i = 0; i < sizeof(arithmetic_symbols) / sizeof(arithmetic_symbols[0]); i++)
    {
        if (token == arithmetic_symbols[i][0])
        {
            return 1;
        }
    }

    return 0;
}

char *get_token(FILE *fp)
{
    int c;
    char *token = malloc(100);
    do
    {
        c = fgetc(fp);
    } while (isspace(c));

    if (c == EOF)
    {
        return NULL;
    }

    if (isalpha(c) || c == '_')
    {
        int i = 0;
        token[i++] = c;
        while (isalnum(c = fgetc(fp)) || c == '_')
        {
            token[i++] = c;
        }
        if (c != EOF)
            ungetc(c, fp);
        token[i] = '\0';
        return token;
    }

    if (isdigit(c))
    {
        int i = 0;
        token[i++] = c;
        while (isdigit(c = fgetc(fp)))
        {
            token[i++] = c;
        }
        if (c != EOF)
            ungetc(c, fp);
        token[i] = '\0';
        return token;
    }

    if (is_symbol(c))
    {
        int i = 0;
        token[i++] = c;
        token[i] = '\0';
        return token;
    }
}

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
            else
            {
                mov_ebx(atoi(var));
                mul_reg32(REG_EAX, REG_EBX);
            }
            free(var);
        }

        next_token = get_token(file);
    }
}

void parse_create_function(FILE *file, char *token)
{
    char *type = get_token(file);
    char *name = get_token(file);

    printf("func %s %s\n", type, name);

    create_label(name);
    create_new_stack();

    free(type);
    free(name);
    free(token);
}

void parse_create_return(FILE *file, char *token)
{
    char *val = get_token(file);

    printf("return %s\n", val);

    get_var(REG_EAX, "a");
    restore_stack();
    ret();

    free(val);
    free(token);
}

void parse_create_int(FILE *file, char *token)
{
    char *name = get_token(file);
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
    get_token(file); // skip '='
    char *value = get_token(file);
    uint32_t val = 0;

    if (does_var_exist(value))
    {
        get_var(REG_EAX, value);

        parse_next_values_arithemtic(file);

        set_var_with_reg(token, REG_EAX);
        printf("int %s = %s\n", token, value);
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

void parse_ifs(FILE *file, char *token)
{
    char *left_condition = get_token(file);
    char *condition = get_token(file);
    char *right_condition = get_token(file);

    if (strcmp(condition, "=") == 0)
    {
        printf("if %s %s %s\n", left_condition, condition, right_condition);

        if (does_var_exist(left_condition))
        {
            get_var(REG_EAX, left_condition);
        }
        else
        {
            uint32_t val = atoi(left_condition);
            mov_eax(val);
        }

        if (does_var_exist(right_condition))
        {
            get_var(REG_EBX, right_condition);
        }
        else
        {
            uint32_t val = atoi(right_condition);
            mov_ebx(val);
        }

        cmp_reg32(REG_EAX, REG_EBX);
        jump_if_not_equal("if1");

        token = get_token(file);
        while (strcmp(token, "}") != 0)
        {
            parse_it(token, file);
            token = get_token(file);
        }
        create_label("if1");
    }
}

void parse_fors(FILE *file, char *token)
{
    token = get_token(file);
    parse_it(token, file);

    char *left_condition = get_token(file);
    char *condition = get_token(file);
    char *right_condition = get_token(file);
    printf("for %s %s %s\n", left_condition, condition, right_condition);
}

int parse_it(char *token, FILE *file)
{
    if (strcmp(token, "func") == 0)
    {
        parse_create_function(file, token);
        return 1;
    }

    if (strcmp(token, "return") == 0)
    {
        parse_create_return(file, token);
        return 1;
    }

    if (strcmp(token, "int") == 0)
    {
        parse_create_int(file, token);
        return 1;
    }

    // conditionals
    if (strcmp(token, "if") == 0)
    {
        parse_ifs(file, token);
        return 1;
    }

    if (strcmp(token, "for") == 0)
    {
        parse_fors(file, token);
        return 1;
    }

    // needs to be after all the initializers of variables
    int ident_question = does_var_exist(token);

    if (ident_question == 1)
    {
        parse_int_setter(file, token);
        return 1;
    }
}

void start_parsing(char *filename)
{
    printf("Parsing file %s\n\n\n\n\n\n\n\n", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    char *token;
    while ((token = get_token(file)) != NULL)
    {
        if (parse_it(token, file) == 1)
        {
            continue;
        }
    }

    printf("\n\n\n\n\n\nFinished parsing file %s\n", filename);

    fclose(file);
}

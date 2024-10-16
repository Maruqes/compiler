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

uint32_t parse_next_values_arithemtic(FILE *file, uint32_t *val)
{
    char *next_token = get_token(file);

    while (next_token[0] != ';')
    {
        if (next_token[0] == '+')
        {
            char *var = get_token(file);
            *val += atoi(var);
            free(var);
        }
        else if (next_token[0] == '-')
        {
            char *var = get_token(file);
            *val -= atoi(var);
            free(var);
        }
        else if (next_token[0] == '/')
        {
            char *var = get_token(file);
            *val /= atoi(var);
            free(var);
        }
        else if (next_token[0] == '*')
        {
            char *var = get_token(file);
            *val *= atoi(var);
            free(var);
        }
        else if (next_token[0] == '^')
        {
            char *var = get_token(file);
            *val = pow(*val, atoi(var));
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

    printf("int %s = %s\n", name, value);

    create_var(name, 4);
    set_var(name, atoi(value));

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
        printf("EXISTE %s\n", value);
        get_var(REG_EAX, value);
        set_var_with_reg(token, REG_EAX);
    }
    else
    {
        val = atoi(value);
        parse_next_values_arithemtic(file, &val);
        set_var(token, val);
    }

    free(value);
    free(token);
}

void start_parsing(char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    char *token;
    while ((token = get_token(file)) != NULL)
    {

        if (strcmp(token, "func") == 0)
        {
            parse_create_function(file, token);
            continue;
        }

        if (strcmp(token, "return") == 0)
        {
            parse_create_return(file, token);
            continue;
        }

        if (strcmp(token, "int") == 0)
        {
            parse_create_int(file, token);
            continue;
        }

        // needs to be after all the initializers of variables
        int ident_question = does_var_exist(token);

        if (ident_question == 1)
        {
            parse_int_setter(file, token);
            continue;
        }

        free(token);
    }

    fclose(file);
}

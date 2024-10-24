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
#include "parser_help.h"
#include "int/parser_int.h"
/*
todo crl

whiles
logical tipo && ! ||
funcoes por default

extra dar acesso a umas funcs ai do assembly mm

functions -> feito ja tem returns de ints
*/

char *funcs_tokens[] = {"func", "endfunc", "return", "for", "endfor"};
char *vars_tokens[] = {"int"};
char *symbol_tokens[] = {";", "=", "<", "(", "{", "}", ")", ">", "=="};
char *arithmetic_symbols[] = {"+", "-", "*", "/", "^"};
char **token_save;

uint64_t labels_count = 0;

char *create_temp_label()
{
    char *label = malloc(100);
    sprintf(label, "temp_label_%ld", labels_count);
    labels_count++;
    return label;
}

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

void parse_create_function(FILE *file, char *token)
{
    char *type = get_token(file);
    char *name = get_token(file);

    printf("func %s %s\n", type, name);

    create_label(name);
    create_new_stack();

    add_function(name, type);

    free(type);
    free(name);
    free(token);
}

void parse_create_return(FILE *file, char *token)
{
    char *val = get_token(file);

    if (does_var_exist(val))
    {
        get_var(REG_EAX, val);
    }
    else
    {
        mov_eax(atoi(val));
    }

    printf("return %s\n", val);

    restore_stack();
    ret();

    free(val);
    free(token);
}

void parse_ifs(FILE *file, char *token)
{
    char *left_condition = get_token(file);
    char *condition = get_token(file);
    char *right_condition = get_token(file);

    char *temp_label_name = create_temp_label();

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

        jump_if_not_equal(temp_label_name);

        token = get_token(file);
        while (strcmp(token, "}") != 0)
        {
            parse_it(token, file);
            token = get_token(file);
        }
        create_label(temp_label_name);
    }
    free(left_condition);
    free(condition);
    free(right_condition);
    free(token);

    free(temp_label_name);
}

void parse_fors(FILE *file, char *token)
{
    token = get_token(file);
    parse_it(token, file); // for i = 0;

    char *left_condition = get_token(file);
    char *condition = get_token(file);
    char *right_condition = get_token(file);
    printf("for %s %s %s\n", left_condition, condition, right_condition); // i < 10;

    get_token(file); // skip ';'

    char *for1Label = create_temp_label();
    char *for2Label = create_temp_label();
    char *endfor = create_temp_label();

    jmp(for2Label);

    token = get_token(file);
    create_label(for1Label);
    parse_it(token, file); // i = i + 1;

    create_label(for2Label);
    if ((condition[0] == '<'))
    {
        if (does_var_exist(left_condition))
        {
            get_var(REG_EAX, left_condition);
        }
        else
        {
            mov_eax(atoi(left_condition));
        }

        if (does_var_exist(right_condition))
        {
            get_var(REG_EBX, right_condition);
        }
        else
        {
            mov_ebx(atoi(right_condition));
        }

        cmp_reg32(REG_EAX, REG_EBX);
        jump_if_greater_or_equal(endfor);
    }

    token = get_token(file);
    while (strcmp(token, "}") != 0)
    {
        parse_it(token, file);
        token = get_token(file);
    }
    jmp(for1Label);

    create_label(endfor);

    free(left_condition);
    free(condition);
    free(right_condition);
    free(token);
    free(for1Label);
    free(for2Label);
    free(endfor);
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
        printf("\n\n\nFOR LOOP\n");
        parse_fors(file, token);
        return 1;
    }

    // assumed that the token is a function call
    if (is_a_function(token))
    {
        printf("Calling function %s\n", token);
        call(token);
        return 1;
    }

    // assumed that the token is a variable
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

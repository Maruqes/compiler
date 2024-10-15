#include "parser.h"
#include <ctype.h>
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
char *symbol_tokens[] = {";", "=", "+", "<", "(", "{", "}", ")", ">", "=="};

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
            char *type = get_token(file);
            char *name = get_token(file);

            printf("func %s %s\n", type, name);

            create_label(name);
            create_new_stack();

            free(type);
            free(name);
            free(token);
            continue;
        }

        if (strcmp(token, "return") == 0)
        {
            char *val = get_token(file);

            printf("return %s\n", val);

            get_var(REG_EAX, "a");
            restore_stack();
            ret();

            free(val);
            free(token);
            continue;
        }

        if (strcmp(token, "int") == 0)
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
            continue;
        }

        // needs to be after all the initializers of variables
        int ident_question = does_var_exist(token);

        if (ident_question == 1)
        {
            get_token(file); // skip '='
            char *value = get_token(file);

            set_var(token, atoi(value));

            free(value);
            free(token);
            continue;
        }

        free(token);
    }

    fclose(file);
}

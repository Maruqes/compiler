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

extra dar acesso a umas funcs ai do assembly mm
function params
ands ors not etc

se as vars do escopo atual ja nao forem usadas(foram usadas num if for exemplo) dar chance de reutilizar
pensar em varios ifs nested

functions -> feito ja tem returns de ints
arrays
negative numbers
incrementation/decrementation
contantes para string e int
*/

char *funcs_tokens[] = {"func", "endfunc", "return", "for", "endfor"};
char *vars_tokens[] = {"int"};
char *symbol_tokens[] = {
    ";",
    "=",
    "<",
    "(",
    "{",
    "}",
    ")",
    ">",
    "*",
    "&",
    "!",
    "[",
    "]",
    ",",
    "'"};
char *arithmetic_symbols[] = {"+", "-", "*", "/", "^"};
char **token_save;

uint64_t labels_count = 0;

char *create_temp_label()
{
    char *label = malloc(100);
    sprintf(label, "temp_label_%lu", labels_count);
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

void create_comparion_bytes(char *condition, char *temp_label_name)
{
    if (strcmp(condition, "=") == 0)
    {
        jump_if_not_equal(temp_label_name);
        return;
    }
    if (strcmp(condition, "<") == 0)
    {
        jump_if_greater_or_equal(temp_label_name);
        return;
    }
    if (strcmp(condition, ">") == 0)
    {
        jump_if_less_or_equal(temp_label_name);
        return;
    }
    if (strcmp(condition, "!") == 0)
    {
        jump_if_equal(temp_label_name);
        return;
    }
    printf("Error: Condition %s not found\n", condition);
    exit(EXIT_FAILURE);
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
        free(token);
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

    if (c == '"')
    {
        int i = 0;
        while ((c = fgetc(fp)) != '"' && c != EOF)
        {
            if (c == '\\')
            {
                c = fgetc(fp);
                if (c == 'n')
                {
                    token[i++] = '\n';
                    continue;
                }
                else if (c == '0')
                {
                    token[i++] = '\0';
                    continue;
                }
            }
            token[i++] = c;
        }
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

    free(token);
    return NULL;
}

void parse_create_function(FILE *file)
{
    char *type = get_token(file);
    if (checkFuncType(type) == 1)
    {
        printf("Error: Function return type must be int\n");
        exit(1);
    }
    char *name = get_token(file);

    printf("func %s %s\n", type, name);

    create_label(name);
    create_new_stack();

    add_function(name, type);

    set_current_scope(name);

    free(type);
    free(name);
}

// return in EAX
void parse_create_return(FILE *file)
{
    parse_after_equal(file);

    printf("return \n");

    restore_stack();
    ret();
}

void parse_ifs(FILE *file)
{
    char *left_condition = get_token(file);
    char *condition = get_token(file);
    char *right_condition = get_token(file);

    char *temp_label_name = create_temp_label();

    printf("if %s %s %s\n", left_condition, condition, right_condition);

    parse_data_types(file, left_condition, REG_EAX);
    parse_data_types(file, right_condition, REG_EBX);

    cmp_reg32(REG_EAX, REG_EBX);
    create_comparion_bytes(condition, temp_label_name);

    char *token = get_token(file);
    while (strcmp(token, "}") != 0)
    {
        parse_it(token, file);
        token = get_token(file);
    }
    create_label(temp_label_name);

    free(left_condition);
    free(condition);
    free(right_condition);
    free(token);
    free(temp_label_name);
}

void parse_fors(FILE *file)
{
    char *token = get_token(file);
    parse_it(token, file); // for i = 0;

    char *left_condition = get_token(file);
    char *condition = get_token(file);
    char *right_condition = get_token(file);
    printf("for %s %s %s\n", left_condition, condition, right_condition); // i < 10;

    get_check_free_semicolon(file);

    char *for1Label = create_temp_label();
    char *for2Label = create_temp_label();
    char *endfor = create_temp_label();

    jmp(for2Label);

    token = get_token(file);
    create_label(for1Label);
    parse_it(token, file); // i = i + 1;

    create_label(for2Label);

    parse_data_types(file, left_condition, REG_EAX);
    parse_data_types(file, right_condition, REG_EBX);

    cmp_reg32(REG_EAX, REG_EBX);
    create_comparion_bytes(condition, endfor);

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

void parse_while(FILE *file)
{
    char *left_condition = get_token(file);
    char *condition = get_token(file);
    char *right_condition = get_token(file);

    char *temp_label_name = create_temp_label();
    char *temp_label_name_end = create_temp_label();

    printf("while %s %s %s\n", left_condition, condition, right_condition);

    create_label(temp_label_name);

    parse_data_types(file, left_condition, REG_EAX);
    parse_data_types(file, right_condition, REG_EBX);

    cmp_reg32(REG_EAX, REG_EBX);
    create_comparion_bytes(condition, temp_label_name_end);

    char *token = get_token(file);
    while (strcmp(token, "}") != 0)
    {
        parse_it(token, file);
        token = get_token(file);
    }
    jmp(temp_label_name);
    create_label(temp_label_name_end);

    free(left_condition);
    free(condition);
    free(right_condition);
    free(token);

    free(temp_label_name);
    free(temp_label_name_end);
}

void parse_create_constant(FILE *file)
{
    // can be -> const int a = 10;  const string a = "hello";
    char *type = get_token(file);
    char *name = get_token(file);
    char *to_free = get_token(file); // skip '='
    free(to_free);
    char *value = get_token(file);

    if (strcmp(type, "int32") == 0)
    {
        printf("Creating uint32 %s with value %s\n", name, value);
        create_constant_uint32_before(name, atoi(value));
        free(value);
    }
    else if (strcmp(type, "string") == 0)
    {
        printf("Creating string %s with value %s\n", name, value);
        create_constant_string_before(name, value);
    }
    else
    {
        printf("Error: Type %s not found\n", type);
        exit(EXIT_FAILURE);
    }
    // dont free name and value because they are used in the constant
    free(type);
}

// should free token;
int parse_it(char *token, FILE *file)
{
    if (strcmp(token, "func") == 0)
    {
        parse_create_function(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "return") == 0)
    {
        parse_create_return(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "int") == 0)
    {
        parse_create_int(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "const") == 0)
    {
        parse_create_constant(file);
        free(token);
        return 1;
    }

    // conditionals
    if (strcmp(token, "if") == 0)
    {
        parse_ifs(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "for") == 0)
    {
        printf("\n\n\nFOR LOOP\n");
        parse_fors(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "while") == 0)
    {
        printf("\n\n\nWHILE LOOP\n");
        parse_while(file);
        free(token);
        return 1;
    }

    if (token[0] == '*')
    {
        parse_set_value_in_the_pointer_address(file);
        return 1;
    }

    // assumed that the token is a function call
    if (is_a_function(token))
    {
        printf("Calling function %s\n", token);
        call(token);
        free(token);
        return 1;
    }

    if (is_a_uint32_arr_beforeconstant(token))
    {
        printf("setting uint32 array value %s\n", token);
        parse_int_array_value_setter(file, token);
        return 1;
    }

    if (is_a_string_beforeconstant(token))
    {
        printf("setting string value %s\n", token);
        parse_string_array_value_setter(file, token);
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
    else
    {
        if (!is_symbol(token[0]))
        {
            printf("Error parse_it: Symbol %s not found\n", token);
            exit(EXIT_FAILURE);
        }
    }
    free(token);
    return 0;
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
        if (parse_it(token, file) == 0)
        {
        }
    }

    printf("\n\n\n\n\n\nFinished parsing file %s\n", filename);

    fclose(file);
}

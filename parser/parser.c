#include "parser.h"
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
#include "parser_help.h"
#include "int/parser_int.h"
#include "../asm_parser/asm_parser.h"
#include "../functions/bFunctions32/bFunctions32.h"
#include "../functions/bFunctions16/bFunctions16.h"
#include "../functions/bFunctions8/bFunctions8.h"
/*
extra dar acesso a umas funcs ai do assembly mm
function params
ands ors not etc

se as vars do escopo atual ja nao forem usadas(foram usadas num if for exemplo) dar chance de reutilizar
pensar em varios ifs nested

functions -> feito ja tem returns de ints
arrays
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
    ".",
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

// system with a memory allocation with address pushed to stack
void get_params(FILE *file)
{
    char *token = get_token(file);
    free(token);

    int params_count = 0;

    token = get_token(file);
    while (strcmp(token, ")") != 0)
    {
        if (strcmp(token, ",") == 0)
        {
            free(token);
            token = get_token(file);
        }
        int paramType = checkFuncType(token);
        if (paramType != DD && paramType != DW && paramType != DB)
        {
            printf("Error:  Unknown param get_params %s\n", token);
            exit(1);
        }
        char *name = get_token(file);

        if (name[0] == '*')
        {
            free(name);
            name = get_token(file);
        }
        printf("param %s\n", name);
        printf("TYPE %d\n", paramType);
        mov_reg32(REG_EDX, 0); // limpar reg edx

        if (paramType == DD)
        {
            create_var(name, 4, 4);
            mov32_r_mi(REG_EDX, REG_EAX, params_count);
            params_count += 4;
            set_var_with_reg(name, REG_EDX);
        }
        else if (paramType == DW)
        {
            create_var(name, 2, 2);
            mov16_r_mi(REG_DX, REG_EAX, params_count);
            set_var_with_reg(name, REG_DX);
            params_count += 4;
        }
        else if (paramType == DB)
        {
            create_var(name, 1, 1);
            mov8_r_mi(REG_DL, REG_EAX, params_count);
            set_var_with_reg(name, REG_DL);
            params_count += 4;
        }

        free(name);
        free(token);
        token = get_token(file);
    }
    free(token);
}

void parse_create_function(FILE *file)
{
    char *name = get_token(file);

    printf("func %s\n", name);

    create_label(name);
    create_new_stack();

    add_function(name);
    set_current_scope(name);

    get_params(file);
    push_eax(); // pusha params addr, this value ios the addr of alloc memory where params are, this needs to be freed

    free(name);
}

// return in EAX
void parse_create_return(FILE *file)
{

    pop_ebx(); // popa params addr

    push_eax(); // pusha return value   ha casos em que o return value esta apenas no eax->
    /*
        ...
        mov eax, 10
        return;

        o valor retornado esta em eax e nao é sobreescrito pelo parse_after_equal
    */

    freeMemoryASM(NUMBER_OF_PAGES, REG_EBX);

    pop_eax(); // popa return value

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

    parse_data_types(file, left_condition, REG_EDX);
    parse_data_types(file, right_condition, REG_EBX);

    cmp_reg32(REG_EDX, REG_EBX);
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

    parse_data_types(file, left_condition, REG_EDX);
    parse_data_types(file, right_condition, REG_EBX);

    cmp_reg32(REG_EDX, REG_EBX);
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

    parse_data_types(file, left_condition, REG_EDX);
    parse_data_types(file, right_condition, REG_EBX);

    cmp_reg32(REG_EDX, REG_EBX);
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

    if (strcmp(token, "dd") == 0)
    {
        parse_create_int(file, DD);
        free(token);
        return 1;
    }

    if (strcmp(token, "dw") == 0)
    {
        parse_create_int(file, DW);
        free(token);
        return 1;
    }

    if (strcmp(token, "db") == 0)
    {
        parse_create_int(file, DB);
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

    if (strcmp(token, "asm") == 0)
    {
        parse_asm_function(file);
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
        parse_functions(file, token);
        free(token);
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

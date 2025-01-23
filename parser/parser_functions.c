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
#include "../functions/bFunctions16/bFunctions16.h"
#include "../functions/bFunctions8/bFunctions8.h"

// Função para validar se a string é um número
int is_valid_number(const char *str)
{
    // Ignorar espaços iniciais
    while (isspace((unsigned char)*str))
        str++;

    // Verificar se começa com '+' ou '-'
    if (*str == '+' || *str == '-')
        str++;

    // Garantir que pelo menos um dígito está presente
    if (!isdigit((unsigned char)*str))
        return 0;

    // Percorrer os restantes caracteres
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
            return 0;
        str++;
    }

    return 1;
}

Function_struct *functions;
uint32_t functions_count = 0;

char *parse_until_charset(FILE *file, char *charset);

int get_check_free_semicolon(FILE *f)
{
    char *token = get_token(f);
    if (token[0] != ';')
    {
        printf("Error: Expected ';'\n");
        exit(1);
    }
    free(token);
    return 1;
}


void set_param_manually(int *params_count)
{
    push_reg(REG_EBX);
    (*params_count)++;
    printf("param\n");
}

// functions
int set_params(FILE *file)
{
    char *left_bracket = get_token(file);
    if (left_bracket[0] != '(')
    {
        printf("Error: Expected '('\n");
        exit(1);
    }
    uint32_t params_count = 0;

    char *ret_from_func = parse_until_charset(file, "),");
    mov_reg32_reg32(REG_EBX, REG_EAX);
    set_param_manually(&params_count);
    while (strcmp(ret_from_func, ")") != 0)
    {
        ret_from_func = parse_until_charset(file, "),");
        mov_reg32_reg32(REG_EBX, REG_EAX);
        set_param_manually(&params_count);
    }

    free(ret_from_func);
    return params_count;
}

void parse_functions(FILE *file, char *token)
{
    printf("Parsing params for func %s\n", token);
    int param = set_params(file);
    call(token);
    if (param != 0)
    {
        add(REG_ESP, param * 4);
    }
}

int is_a_function(char *token)
{
    for (int i = 0; i < functions_count; i++)
    {
        if (strcmp(functions[i].name, token) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void add_function(char *name)
{
    printf("Adding function %s\n", name);
    functions = realloc(functions, sizeof(Function_struct) * (functions_count + 1));
    if (functions == NULL)
    {
        printf("Error: Could not allocate memory for functions\n");
        exit(1);
    }
    functions[functions_count].name = malloc(strlen(name) + 1);
    strcpy(functions[functions_count].name, name);
    functions[functions_count].return_type = "DD";   //becouse all function return 4 bytes
    functions_count++;
}

void free_functions()
{
    for (uint32_t i = 0; i < functions_count; i++)
    {
        free(functions[i].name);
    }
    free(functions);
}

char *current_scope = NULL;

void free_current_scope()
{
    if (current_scope != NULL)
    {
        free(current_scope);
    }
}

void set_current_scope(char *scope)
{
    free_current_scope();
    current_scope = malloc(strlen(scope) + 1);
    strcpy(current_scope, scope);
    create_new_scope_var(scope);
}

char *get_current_scope()
{
    return current_scope;
}

/*
inside functions
file
token is name of function
reg is the register to store the return value
*/
int parse_inside_functions(FILE *file, char *token, uint8_t reg)
{
    if (strcmp(token, "CreateString") == 0)
    {
        parse_CreateString(file, reg);
        return 1;
    }
    return 0;
}

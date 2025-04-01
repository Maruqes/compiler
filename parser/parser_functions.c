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

uint32_t parse_hexadecimal(char *token)
{
    // Salta a parte "x" ou "0x", dependendo do caso
    token += 1; // se tiveres só "xFFFFFFFF"
    // ou token += 2; se tiveres "0xFFFFFFFF"

    uint32_t val = 0;
    for (int i = 0; token[i] != '\0'; i++)
    {
        char c = token[i];
        uint32_t digit = 0;

        if (c >= '0' && c <= '9')
        {
            digit = c - '0';
        }
        else if (c >= 'A' && c <= 'F')
        {
            digit = c - 'A' + 10;
        }
        else if (c >= 'a' && c <= 'f')
        {
            digit = c - 'a' + 10;
        }
        else
        {
            // Tratamento de erro se o caracter não for hex
        }

        val = val * 16 + digit;
    }

    return val;
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
    functions[functions_count].return_type = "DD"; // becouse all function return 4 bytes
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

// functions
typedef struct Paramtemp
{
    char *name;
    int type;
} Paramtemp;

// system with a memory allocation with address pushed to stack
void get_params(FILE *file)
{
    char *token = get_token(file);
    free(token);

    int params_count = 0;
    Paramtemp *params = NULL;

    token = get_token(file);
    while (strcmp(token, ")") != 0)
    {
        if (strcmp(token, ",") == 0)
        {
            free(token);
            token = get_token(file);
        }
        int paramType = check_type(token);
        if (paramType == 0)
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

        Paramtemp temp;
        temp.name = malloc(strlen(name) + 1);
        strcpy(temp.name, name);
        temp.type = paramType;

        params = realloc(params, sizeof(Paramtemp) * (params_count + 1));
        params[params_count] = temp;

        params_count++;

        free(name);
        free(token);
        token = get_token(file);
    }

    for (int i = 0; i < params_count; i++)
    {
        Paramtemp param = params[params_count - i - 1];
        add_var_to_array_with_offset(param.name, get_type_length(param.type), get_current_scope(), get_type_length(param.type), -8 - (i * 4), param.type);
    }

    for (int i = 0; i < params_count; i++)
    {
        free(params[i].name);
    }
    free(params);
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

    free(name);

    // get { and parse block
    char *token = get_token(file);
    if (strcmp(token, "{") != 0)
    {
        printf("token %s\n", token);
        printf("Error: Expected {\n");
        exit(1);
    }

    parse_block(file);
}

// return in EAX
void parse_create_return(FILE *file)
{
    parse_after_equal(file);

    printf("return \n");

    restore_stack();
    ret();
}

/*
inside functions
file
token is name of function
reg is the register to store the return value
*/

void parse_not(FILE *file, uint8_t reg)
{
    char *token = get_token(file);
    if (strcmp(token, "(") != 0)
    {
        printf("Error: Expected '('\n");
        exit(1);
    }
    free(token);
    token = get_token(file);
    get_var(reg, token);
    not(reg); // return value in EAX

    // clear )
    free(token);
    token = get_token(file);
    if (strcmp(token, ")") != 0)
    {
        printf("Error: Expected ')'\n");
        exit(1);
    }
    free(token);
}

// fix better functions inputs
int parse_inside_functions(FILE *file, char *token, uint8_t reg)
{
    if (strcmp(token, "String") == 0)
    {
        parse_CreateString(file, reg);
        return 1;
    }
    else if (strcmp(token, "CString") == 0)
    {
        parse_create_string(file, reg);
        return 1;
    }
    else if (strcmp(token, "CFloat") == 0)
    {
        parse_float(file, reg);
        return 1;
    }
    return 0;
}

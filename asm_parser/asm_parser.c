#include "asm_parser.h"
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
#include "../parser/parser.h"
#include "../parser/parser_help.h"
/*
ao passar valor de variaveis para registers sobreescreve o valor da EAX
*/
int convert_string_to_reg(char *reg)
{
    if (strcmp(reg, "ebp") == 0)
    {
        return REG_EBP;
    }
    if (strcmp(reg, "esp") == 0)
    {
        return REG_ESP;
    }
    if (strcmp(reg, "eax") == 0)
    {
        return REG_EAX;
    }
    if (strcmp(reg, "ebx") == 0)
    {
        return REG_EBX;
    }
    if (strcmp(reg, "ecx") == 0)
    {
        return REG_ECX;
    }
    if (strcmp(reg, "edx") == 0)
    {
        return REG_EDX;
    }
    if (strcmp(reg, "esi") == 0)
    {
        return REG_ESI;
    }
    if (strcmp(reg, "edi") == 0)
    {
        return REG_EDI;
    }
    if (strcmp(reg, "ah") == 0)
    {
        return REG_AH;
    }
    if (strcmp(reg, "bh") == 0)
    {
        return REG_BH;
    }
    if (strcmp(reg, "ch") == 0)
    {
        return REG_CH;
    }
    if (strcmp(reg, "dh") == 0)
    {
        return REG_DH;
    }
    if (strcmp(reg, "al") == 0)
    {
        return REG_AL;
    }
    if (strcmp(reg, "bl") == 0)
    {
        return REG_BL;
    }
    if (strcmp(reg, "cl") == 0)
    {
        return REG_CL;
    }
    if (strcmp(reg, "dl") == 0)
    {
        return REG_DL;
    }
    if (strcmp(reg, "ax") == 0)
    {
        return REG_AX;
    }
    if (strcmp(reg, "bx") == 0)
    {
        return REG_BX;
    }
    if (strcmp(reg, "cx") == 0)
    {
        return REG_CX;
    }
    if (strcmp(reg, "dx") == 0)
    {
        return REG_DX;
    }
    if (strcmp(reg, "si") == 0)
    {
        return REG_SI;
    }
    if (strcmp(reg, "di") == 0)
    {
        return REG_DI;
    }

    return -1;
}

int *asm_get_number(char **str, int pos)
{
    int *res = malloc(2 * sizeof(int));
    if (res == NULL)
    {
        printf("Error: Could not allocate memory for number\n");
        exit(1);
    }
    res[1] = 0;
    if (str[pos][0] == '-')
    {
        if (is_valid_number(str[pos + 1]))
        {
            res[0] = -atoi(str[pos]);
            res[1] = 0;
        }
    }
    else if (is_valid_number(str[pos]))
    {
        res[0] = atoi(str[pos]);
        res[1] = 0;
    }
    else if (does_var_exist(str[pos]))
    {
        // var
        get_var(REG_EAX, str[pos]);
        push_eax();
        res[1] = 2;
    }
    else
    {
        res[1] = -1;
    }
    return res;
}
void asm_mov32(FILE *file, char **tokens)
{
    int reg = convert_string_to_reg(tokens[1]);
    if (reg == -1)
    {
        printf("Error: Register %s not found\n", tokens[1]);
        exit(1);
    }

    int *values = asm_get_number(tokens, 2);
    int value = values[0];
    if (values[1] == 0)
    {

        mov_reg32(reg, value);
        printf("mov %s, %d\n", tokens[1], value);
    }
    else if (values[1] == 2)
    {
        // var
        pop_reg(reg);
    }
    else
    {
        // mov reg reg
        int reg2 = convert_string_to_reg(tokens[2]);
        if (reg2 == -1)
        {
            printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
            exit(1);
        }

        mov_reg32_reg32(reg, reg2);
        printf("mov %s, %s\n", tokens[1], tokens[2]);
    }
    free(values);
}

void asm_mov_reg_offset_value(FILE *file, char **tokens)
{

    int *values = asm_get_number(tokens, 2);
    int *values2 = asm_get_number(tokens, 3);
    int value = values2[0];
    int offset = values[0];
    printf("value: %d, offset: %d\n", value, offset);
    if (values[1] == 0 && values2[1] == 0)
    {
        // mov reg offset value
        int reg = convert_string_to_reg(tokens[1]);
        if (reg == -1)
        {
            printf("Error: Register %s not found\n", tokens[1]);
            exit(1);
        }

        mov_reg_offset_value(reg, offset, value);
        printf("mov %s, off:%d, val:%d\n", tokens[1], offset, value);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
}

void parse_asm_function(FILE *file)
{
    char *token = get_token(file);
    char **tokens = NULL;
    size_t tokens_count = 0;

    while (strcmp(token, ")") != 0)
    {
        tokens = realloc(tokens, sizeof(char *) * (tokens_count + 1));
        if (tokens == NULL)
        {
            printf("Error: Could not allocate memory for tokens\n");
            exit(1);
        }
        tokens[tokens_count] = token;
        tokens_count++;
        token = get_token(file);
    }
    free(token);

    // remove the (
    free(tokens[0]);
    for (size_t i = 0; i < tokens_count - 1; i++)
    {
        tokens[i] = tokens[i + 1];
        printf("%ld -> %s\n", i, tokens[i]);
    }
    tokens_count--;

    if (strcmp(tokens[0], "mov32") == 0)
    {
        asm_mov32(file, tokens);
    }
    else if (strcmp(tokens[0], "syscall") == 0)
    {
        our_syscall();
    }
    else if (strcmp(tokens[0], "mov_reg_offset_value") == 0)
    {
        asm_mov_reg_offset_value(file, tokens);
    }
    else
    {
        printf("Error: Function %s not found\n", tokens[0]);
        exit(1);
    }

    // Free the tokens array
    for (size_t i = 0; i < tokens_count; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}

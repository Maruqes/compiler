#include "asm_parser.h"
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
#include "../parser/parser.h"
#include "../parser/parser_help.h"
#include "../functions/bFunctions8/bFunctions8.h"

void asm_mov8(FILE *file, char **tokens)
{
    int reg = convert_string_to_reg(tokens[1]);
    // checking if reg is valid inside the && ifs

    int *values = asm_get_number(tokens, 2, reg);
    int value = values[0];
    if (values[1] == 0 && reg != -1)
    {

        mov8_r_i(reg, value);
        printf("mov %s, %d\n", tokens[1], value);
    }
    else if (values[1] == 2 && reg != -1)
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

        if (reg == -1)
        {
            // check if a variable
            if (does_var_exist(tokens[1]))
            {
                printf("mov %s, %s\n", tokens[1], tokens[2]);
                set_var_with_reg(tokens[1], reg2);
                return;
            }
        }

        mov8_r_r(reg, reg2);
        printf("mov %s, %s\n", tokens[1], tokens[2]);
    }
    free(values);
}

void asm_mov8_mi_i(FILE *file, char **tokens)
{

    int *values = asm_get_number(tokens, 2, -1);
    int *values2 = asm_get_number(tokens, 3, -1);
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

        mov8_mi_i(reg, offset, value);
        printf("mov %s, off:%d, val:%d\n", tokens[1], offset, value);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
}

void asm_mov8_r_m(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);

    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    mov8_r_m(reg1, reg2);
    printf("mov %s, %s\n", tokens[1], tokens[2]);
}

void asm_mov8_m_i(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int *values = asm_get_number(tokens, 2, -1);
    int value = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1)
        {
            printf("Error: Register %s not found\n", tokens[1]);
            exit(1);
        }

        mov8_m_i(reg1, value);
        printf("mov %s, %d\n", tokens[1], value);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

void asm_mov8_m_r(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);

    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    mov8_m_r(reg1, reg2);
    printf("mov %s, %s\n", tokens[1], tokens[2]);
}

void asm_mov8_mr_r(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int reg3 = convert_string_to_reg(tokens[3]);

    if (reg1 == -1 || reg2 == -1 || reg3 == -1)
    {
        printf("Error: Register %s or %s or %s not found\n", tokens[1], tokens[2], tokens[3]);
        exit(1);
    }

    mov8_mr_r(reg1, reg2, reg3);
    printf("mov [%s + %s], %s\n", tokens[1], tokens[2], tokens[3]);
}

void asm_mov8_r_mr(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int reg3 = convert_string_to_reg(tokens[3]);

    if (reg1 == -1 || reg2 == -1 || reg3 == -1)
    {
        printf("Error: Register %s or %s or %s not found\n", tokens[1], tokens[2], tokens[3]);
        exit(1);
    }

    mov8_r_mr(reg1, reg2, reg3);
    printf("mov %s, [%s + %s]\n", tokens[1], tokens[2], tokens[3]);
}

void asm_mov8_mr_i(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int *values = asm_get_number(tokens, 3, -1);
    int value = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1 || reg2 == -1)
        {
            printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
            exit(1);
        }

        mov8_mr_i(reg1, reg2, value);
        printf("mov %s, [%s + %d]\n", tokens[1], tokens[2], value);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

void asm_mov8_mi_r(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[3]);
    int *values = asm_get_number(tokens, 2, -1);
    int offset = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1 || reg2 == -1)
        {
            printf("Error: Register %s or %s not found\n", tokens[1], tokens[3]);
            exit(1);
        }

        mov8_mi_r(reg1, offset, reg2);
        printf("mov %s, [%d + %s]\n", tokens[1], offset, tokens[3]);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

void asm_mov8_r_mi(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int *values = asm_get_number(tokens, 3, -1);
    int offset = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1 || reg2 == -1)
        {
            printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
            exit(1);
        }

        mov8_r_mi(reg1, reg2, offset);
        printf("mov %s, %s + %d\n", tokens[1], tokens[2], offset);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

int parse_movs8(FILE *file, char **tokens)
{
    if (strcmp(tokens[0], "mov8") == 0)
    {
        asm_mov8(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_mi_i") == 0)
    {
        asm_mov8_mi_i(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_r_m") == 0)
    {
        asm_mov8_r_m(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_m_i") == 0)
    {
        asm_mov8_m_i(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_m_r") == 0)
    {
        asm_mov8_m_r(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_mr_r") == 0)
    {
        asm_mov8_mr_r(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_r_mr") == 0)
    {
        asm_mov8_r_mr(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_mr_i") == 0)
    {
        asm_mov8_mr_i(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_mi_r") == 0)
    {
        asm_mov8_mi_r(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov8_r_mi") == 0)
    {
        asm_mov8_r_mi(file, tokens);
        return 1;
    }
    else
    {
        return 0;
    }
}
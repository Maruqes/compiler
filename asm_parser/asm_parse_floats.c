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
#include "../functions/bFunctions32/bFunctions32.h"

void asm_mov_x_r(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    mov_x_r(reg1, reg2);
    printf("movss %s, %s \n", tokens[1], tokens[2]);
}

void asm_mov_r_x(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    mov_r_x(reg1, reg2);
    printf("movss %s, %s \n", tokens[1], tokens[2]);
}

void asm_cvtss2si(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    cvtss2si(reg1, reg2);
    printf("cvtss2si %s, %s \n", tokens[1], tokens[2]);
}

int parse_floats(FILE *file, char **tokens)
{
    if (strcmp(tokens[0], "mov_x_r") == 0)
    {
        asm_mov_x_r(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov_r_x") == 0)
    {
        asm_mov_r_x(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "cvtss2si") == 0)
    {
        asm_cvtss2si(file, tokens);
        return 1;
    }
    return 0;
}
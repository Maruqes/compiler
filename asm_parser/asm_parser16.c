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

void asm_mov16(FILE *file, char **tokens)
{
    int reg = convert_string_to_reg(tokens[1]);
    // checking if reg is valid inside the && ifs

    int *values = asm_get_number(tokens, 2);
    int value = values[0];
    if (values[1] == 0 && reg != -1)
    {

        mov_reg16(reg, value);
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

        mov_reg16_reg16(reg, reg2);
        printf("mov16 %s, %s\n", tokens[1], tokens[2]);
    }
    free(values);
}

int parse_movs16(FILE *file, char **tokens)
{
    if (strcmp(tokens[0], "mov16") == 0)
    {
        asm_mov16(file, tokens);
        return 1;
    }
    else
    {
        return 0;
    }
}
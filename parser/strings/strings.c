#include "strings.h"
#include <ctype.h>
#include <math.h>
#include "../../functions/functions.h"
#include "../../types/strings.h"
#include "../../mov_reg_reg/mov_reg_reg.h"
#include "../../push_pop/push_pop.h"
#include "../../16_code/functions_16.h"
#include "../../8low_code/functions_8low.h"
#include "../../8high_code/functions_8high.h"
#include "../../jumps/jumps.h"
#include "../../arithmetic/arithmetic.h"
#include "../../logic/logic.h"
#include "../../variables/variables.h"
#include "../parser.h"
#include "../parser_help.h"
#include "../../functions/bFunctions32/bFunctions32.h"
#include "../../functions/bFunctions8/bFunctions8.h"

// returns in  reg address
void parse_CreateString(FILE *file, uint8_t reg)
{
    char *left_bracket = get_token(file);
    if (left_bracket[0] != '(')
    {
        printf("Error: Expected '('\n");
        exit(1);
    }
    free(left_bracket);

    char *str = get_token(file); // T= "

    allocMemoryASM(1);
    for (int j = 0; j < strlen(str); j++)
    {
        mov8_mi_i(REG_EAX, j, str[j]);
    }

    mov_reg32_reg32(reg, REG_EAX);

    char *right_bracket = get_token(file);
    if (right_bracket[0] != ')')
    {
        printf("Error: Expected ')'\n");
        exit(1);
    }
    free(right_bracket);

    free(str);
}

char *create_random_var_name(char *prefix)
{
    char *var_name = malloc(20);
    snprintf(var_name, 20, "%s_%d", prefix, rand() % 10000);
    return var_name;
}

void parse_create_string(FILE *file, uint8_t reg)
{
    char *left_bracket = get_token(file);
    if (left_bracket[0] != '(')
    {
        printf("Error: Expected '('\n");
        exit(1);
    }
    free(left_bracket);

    char *str = get_token(file); // T= "

    char *varname_copy = create_random_var_name("string");
    create_constant_string_before(varname_copy, str);
    mov_reg32_symbol_address(reg, varname_copy, 0);

    char *right_bracket = get_token(file);
    if (right_bracket[0] != ')')
    {
        printf("Error: Expected ')'\n");
        exit(1);
    }
    free(right_bracket);

    free(str);
}
#include "parser_help.h"
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

Function_struct *functions;
uint32_t functions_count = 0;

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

void add_function(char *name, char *return_type)
{
    functions = realloc(functions, sizeof(Function_struct) * (functions_count + 1));
    functions[functions_count].name = malloc(strlen(name) + 1);
    strcpy(functions[functions_count].name, name);
    functions[functions_count].return_type = return_type;
    functions_count++;
}
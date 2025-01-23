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

union parse_float_union
{
    float f;
    uint32_t i;
};

void parse_float(FILE *fp, char *token, uint8_t reg)
{
    token++; // skip the f
    char *dot = get_token(fp);
    if (dot[0] != '.')
    {
        printf("Error: Expected '.'\n");
        exit(1);
    }
    char *afterDot = get_token(fp);
    if (!is_valid_number(afterDot))
    {
        printf("Error: Expected number\n");
        exit(1);
    }


    char *temp = malloc(strlen(token) + strlen(afterDot) + 2);
    strcpy(temp, token);
    strcpy(temp + strlen(token), ".");
    strcpy(temp + strlen(token) + 1, afterDot);

    union parse_float_union p;
    p.f = strtof(temp, NULL);

    mov_reg32(reg, p.i);


    free(dot);
    free(afterDot);
    free(temp);
}
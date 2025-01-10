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
        printf("%s\n", tokens[i]);
    }
    tokens_count--;

    if (strcmp(tokens[0], "mov32") == 0)
    {
        if (is_valid_number(tokens[2]))
        {
            // mover valor para registrador

            int value = atoi(tokens[2]);
            int reg = convert_string_to_reg(tokens[1]);
            if (reg == -1)
            {
                printf("Error: Register %s not found\n", tokens[1]);
                exit(1);
            }

            if (value == 69)
                value = -1;

            mov_reg32(reg, value);
            printf("mov %s, %d\n", tokens[1], value);
        }
        else
        {
            // mover valor de registrador para registrador
            int reg1 = convert_string_to_reg(tokens[1]);
            int reg2 = convert_string_to_reg(tokens[2]);
            if (reg1 == -1 || reg2 == -1)
            {
                printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
                exit(1);
            }

            mov_reg32_reg32(reg1, reg2);
            printf("mov %s, %s\n", tokens[1], tokens[2]);
        }
    }
    else if (strcmp(tokens[0], "syscall") == 0)
    {
        our_syscall();

        mov_reg_offset_value(REG_EAX, 0, 65);
        mov_reg_offset_value(REG_EAX, 1, 66);
        mov_reg_offset_value(REG_EAX, 2, 67);
        mov_reg_offset_value(REG_EAX, 3, 10);
        mov_reg32_reg32(REG_ECX, REG_EAX);
        mov_eax(4);
        mov_ebx(1);
        mov_edx(4);
        our_syscall();
    }

    // Free the tokens array
    for (size_t i = 0; i < tokens_count; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}

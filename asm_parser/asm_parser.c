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
#include "../functions/bFunctions16/bFunctions16.h"
#include "../functions/bFunctions32/bFunctions32.h"
/*
ao passar valor de variaveis para registers sobreescreve o valor da EAX
*/
// falta fazer passar um reg pra variavel :D
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

// returns the in [0] the value  is [1] = 0 if number, == 2 if variable (pushed to stack, will be poped later) and -1 if invalid
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
    else if (str[pos][0] == '\'')
    {
        res[0] = str[pos + 1][0];
        res[1] = 0;

        if(str[pos + 2][0] != '\''){
            printf("Error: Invalid character\n");
            exit(1);
        }
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
    // checking if reg is valid inside the && ifs

    int *values = asm_get_number(tokens, 2);
    int value = values[0];
    if (values[1] == 0 && reg != -1)
    {

        mov32_r_i(reg, value);
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

        mov32_r_r(reg, reg2);
        printf("mov %s, %s\n", tokens[1], tokens[2]);
    }
    free(values);
}

void asm_mov32_mi_i(FILE *file, char **tokens)
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

        mov32_mi_i(reg, offset, value);
        printf("mov %s, off:%d, val:%d\n", tokens[1], offset, value);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
}

void asm_mov32_r_m(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);

    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    mov32_r_m(reg1, reg2);
    printf("mov %s, %s\n", tokens[1], tokens[2]);
}

void asm_mov32_m_i(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int *values = asm_get_number(tokens, 2);
    int value = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1)
        {
            printf("Error: Register %s not found\n", tokens[1]);
            exit(1);
        }

        mov32_m_i(reg1, value);
        printf("mov %s, %d\n", tokens[1], value);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

void asm_mov32_m_r(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);

    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    mov32_m_r(reg1, reg2);
    printf("mov %s, %s\n", tokens[1], tokens[2]);
}

void asm_mov32_mr_r(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int reg3 = convert_string_to_reg(tokens[3]);

    if (reg1 == -1 || reg2 == -1 || reg3 == -1)
    {
        printf("Error: Register %s or %s or %s not found\n", tokens[1], tokens[2], tokens[3]);
        exit(1);
    }

    mov32_mr_r(reg1, reg2, reg3);
    printf("mov [%s + %s], %s\n", tokens[1], tokens[2], tokens[3]);
}

void asm_mov32_r_mr(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int reg3 = convert_string_to_reg(tokens[3]);

    if (reg1 == -1 || reg2 == -1 || reg3 == -1)
    {
        printf("Error: Register %s or %s or %s not found\n", tokens[1], tokens[2], tokens[3]);
        exit(1);
    }

    mov32_r_mr(reg1, reg2, reg3);
    printf("mov %s, [%s + %s]\n", tokens[1], tokens[2], tokens[3]);
}

void asm_mov32_mr_i(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int *values = asm_get_number(tokens, 3);
    int value = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1 || reg2 == -1)
        {
            printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
            exit(1);
        }

        mov32_mr_i(reg1, reg2, value);
        printf("mov %s, [%s + %d]\n", tokens[1], tokens[2], value);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

void asm_mov32_mi_r(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[3]);
    int *values = asm_get_number(tokens, 2);
    int offset = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1 || reg2 == -1)
        {
            printf("Error: Register %s or %s not found\n", tokens[1], tokens[3]);
            exit(1);
        }

        mov32_mi_r(reg1, offset, reg2);
        printf("mov %s, [%d + %s]\n", tokens[1], offset, tokens[3]);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

void asm_mov32_r_mi(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    int *values = asm_get_number(tokens, 3);
    int offset = values[0];
    if (values[1] == 0)
    {
        if (reg1 == -1 || reg2 == -1)
        {
            printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
            exit(1);
        }

        mov32_r_mi(reg1, reg2, offset);
        printf("mov %s, %s + %d\n", tokens[1], tokens[2], offset);
    }
    else
    {
        printf("Error: Invalid number\n");
        exit(1);
    }
    free(values);
}

void asm_inc(FILE *file, char **tokens)
{
    int reg = convert_string_to_reg(tokens[1]);
    if (reg == -1)
    {
        printf("Error: Register %s not found\n", tokens[1]);
        exit(1);
    }

    inc_reg32(reg);
    printf("inc %s\n", tokens[1]);
}

void asm_dec(FILE *file, char **tokens)
{
    int reg = convert_string_to_reg(tokens[1]);
    if (reg == -1)
    {
        printf("Error: Register %s not found\n", tokens[1]);
        exit(1);
    }

    dec_reg32(reg);
    printf("dec %s\n", tokens[1]);
}

void asm_cmp32(FILE *file, char **tokens)
{
    int reg1 = convert_string_to_reg(tokens[1]);
    int reg2 = convert_string_to_reg(tokens[2]);
    if (reg1 == -1 || reg2 == -1)
    {
        printf("Error: Register %s or %s not found\n", tokens[1], tokens[2]);
        exit(1);
    }

    cmp_reg32(reg1, reg2);
    printf("cmp %s, %s\n", tokens[1], tokens[2]);
}

void asm_push(FILE *file, char **tokens)
{
    int reg = convert_string_to_reg(tokens[1]);
    if (reg == -1)
    {
        printf("Error: Register %s not found\n", tokens[1]);
        exit(1);
    }
    push_reg(reg);
    printf("push %s\n", tokens[1]);
}

void asm_pop(FILE *file, char **tokens)
{
    int reg = convert_string_to_reg(tokens[1]);
    if (reg == -1)
    {
        printf("Error: Register %s not found\n", tokens[1]);
        exit(1);
    }
    pop_reg(reg);
    printf("pop %s\n", tokens[1]);
}

void asm_pusha(FILE *file, char **tokens)
{
    push_reg(REG_EAX);
    push_reg(REG_EBX);
    push_reg(REG_ECX);
    push_reg(REG_EDX);
    push_reg(REG_ESI);
    push_reg(REG_EDI);
    printf("pusha\n");
}

void asm_popa(FILE *file, char **tokens)
{
    pop_reg(REG_EDI);
    pop_reg(REG_ESI);
    pop_reg(REG_EDX);
    pop_reg(REG_ECX);
    pop_reg(REG_EBX);
    pop_reg(REG_EAX);
    printf("popa\n");
}

int parse_movs(FILE *file, char **tokens)
{
    if (strcmp(tokens[0], "mov32") == 0)
    {
        asm_mov32(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_mi_i") == 0)
    {
        asm_mov32_mi_i(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_r_m") == 0)
    {
        asm_mov32_r_m(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_m_i") == 0)
    {
        asm_mov32_m_i(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_m_r") == 0)
    {
        asm_mov32_m_r(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_mr_r") == 0)
    {
        asm_mov32_mr_r(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_r_mr") == 0)
    {
        asm_mov32_r_mr(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_mr_i") == 0)
    {
        asm_mov32_mr_i(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_mi_r") == 0)
    {
        asm_mov32_mi_r(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "mov32_r_mi") == 0)
    {
        asm_mov32_r_mi(file, tokens);
        return 1;
    }
    else
    {
        return 0;
    }
}

int parse_extras(FILE *file, char **tokens)
{
    if (strcmp(tokens[0], "inc") == 0)
    {
        asm_inc(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "dec") == 0)
    {
        asm_dec(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "push") == 0)
    {
        asm_push(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "pop") == 0)
    {
        asm_pop(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "pusha") == 0)
    {
        asm_pusha(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "popa") == 0)
    {
        asm_popa(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "int") == 0)
    {
        int *values = asm_get_number(tokens, 1);
        if (values[1] == 0)
        {
            interrupt_call(values[0]);
            return 1;
        }
        else
        {
            printf("Error: Invalid number\n");
            exit(1);
        }
    }
    else if (strcmp(tokens[0], "syscall") == 0)
    {
        our_syscall();
        return 1;
    }
    else
    {
        return 0;
    }
}
int parse_jumps(FILE *file, char **tokens)
{

    if (strcmp(tokens[0], "create_label") == 0)
    {
        create_label(tokens[1]);
        printf("create_label %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "cmp32") == 0)
    {
        asm_cmp32(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "je") == 0)
    {
        jump_if_equal(tokens[1]);
        printf("je %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "jne") == 0)
    {
        jump_if_not_equal(tokens[1]);
        printf("jne %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "jg") == 0)
    {
        jump_if_greater(tokens[1]);
        printf("jg %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "jge") == 0)
    {
        jump_if_greater_or_equal(tokens[1]);
        printf("jge %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "jl") == 0)
    {
        jump_if_less(tokens[1]);
        printf("jl %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "jle") == 0)
    {
        jump_if_less_or_equal(tokens[1]);
        printf("jle %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "jmp") == 0)
    {
        jmp(tokens[1]);
        printf("jmp %s\n", tokens[1]);
        return 1;
    }
    else if (strcmp(tokens[0], "push") == 0)
    {
        asm_push(file, tokens);
        return 1;
    }
    else if (strcmp(tokens[0], "pop") == 0)
    {
        asm_pop(file, tokens);
        return 1;
    }
    else
    {
        return 0;
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

    int parsed = 0;
    parsed = parsed || parse_movs(file, tokens);
    parsed = parsed || parse_extras(file, tokens);
    parsed = parsed || parse_jumps(file, tokens);
    parsed = parsed || parse_movs16(file, tokens);
    parsed = parsed || parse_movs8(file, tokens);

    if (!parsed)
    {
        printf("Error: Unknown token %s\n", tokens[0]);
        exit(1);
    }

    // Free the tokens array
    for (size_t i = 0; i < tokens_count; i++)
    {
        free(tokens[i]);
    }
    free(tokens);
}

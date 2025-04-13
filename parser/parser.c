#include "parser.h"
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
#include "parser_help.h"
#include "int/parser_int.h"
#include "../asm_parser/asm_parser.h"
#include "../functions/bFunctions32/bFunctions32.h"
#include "../functions/bFunctions8/bFunctions8.h"
#include "structs/structs.h"
/*
extra dar acesso a umas funcs ai do assembly mm
function params
ands ors not etc

se as vars do escopo atual ja nao forem usadas(foram usadas num if for exemplo) dar chance de reutilizar
pensar em varios ifs nested

functions -> feito ja tem returns de ints
arrays
contantes para string e int
*/

char *funcs_tokens[] = {"func", "endfunc", "return", "for", "endfor"};
char *vars_tokens[] = {"int"};
char *symbol_tokens[] = {
    ";",
    "~",
    "=",
    "<",
    "(",
    "{",
    "}",
    ")",
    ">",
    "*",
    "&",
    "|",
    "!",
    "[",
    "]",
    ",",
    ".",
    "#",
    "'"};
char *arithmetic_symbols[] = {"+", "-", "*", "/", "^", "%"};
char **token_save;

uint64_t labels_count = 0;

char *create_temp_label()
{
    char *label = malloc(100);
    sprintf(label, "temp_label_%lu", labels_count);
    labels_count++;
    return label;
}

int parse_it(char *token, FILE *file);

int is_symbol(char token)
{
    for (int i = 0; i < sizeof(symbol_tokens) / sizeof(symbol_tokens[0]); i++)
    {
        if (token == symbol_tokens[i][0])
        {
            return 1;
        }
    }

    for (int i = 0; i < sizeof(arithmetic_symbols) / sizeof(arithmetic_symbols[0]); i++)
    {
        if (token == arithmetic_symbols[i][0])
        {
            return 1;
        }
    }

    return 0;
}

// will parse until '}'
void parse_block(FILE *file)
{
    char *token = get_token(file);
    while (strcmp(token, "}") != 0)
    {
        parse_it(token, file);
        token = get_token(file);
    }
    free(token);
}

// if condition false jump to to_jump
void create_comparison_bytes(char *condition, char *temp_label_name)
{
    if (strcmp(condition, "=") == 0)
    {
        jump_if_not_equal(temp_label_name);
        return;
    }
    if (strcmp(condition, "<") == 0)
    {
        jump_if_greater_or_equal(temp_label_name);
        return;
    }
    if (strcmp(condition, ">") == 0)
    {
        jump_if_less_or_equal(temp_label_name);
        return;
    }
    if (strcmp(condition, "!") == 0)
    {
        jump_if_equal(temp_label_name);
        return;
    }
    printf("Error: Condition %s not found\n", condition);
    exit(EXIT_FAILURE);
}

// push 0 if false 1 if true
void push_comparison_value_to_stack(uint8_t reg1, uint8_t reg2, char *condition)
{
    char *set0 = create_temp_label();
    char *end = create_temp_label();
    // order of regs matters
    cmp_reg32(REG_EBX, REG_EAX);
    create_comparison_bytes(condition, set0);

    mov_eax(1);
    jmp(end);

    create_label(set0);
    mov_eax(0);

    create_label(end);

    push_eax();

    free(set0);
    free(end);
}

void go_back_x_char(int x, FILE *file)
{
    fseek(file, -x, SEEK_CUR);
}

char *get_token(FILE *fp)
{
    int c;
    char *token = malloc(100);
    do
    {
        c = fgetc(fp);
    } while (isspace(c));

    if (c == EOF)
    {
        free(token);
        return NULL;
    }

    if (isalpha(c) || c == '_')
    {
        int i = 0;
        token[i++] = c;
        while (isalnum(c = fgetc(fp)) || c == '_')
        {
            token[i++] = c;
        }
        if (c != EOF)
            ungetc(c, fp);
        token[i] = '\0';
        return token;
    }

    if (isdigit(c))
    {
        int i = 0;
        token[i++] = c;
        while (isdigit(c = fgetc(fp)))
        {
            token[i++] = c;
        }
        if (c != EOF)
            ungetc(c, fp);
        token[i] = '\0';
        return token;
    }

    if (c == '"')
    {
        int i = 0;
        while ((c = fgetc(fp)) != '"' && c != EOF)
        {
            if (c == '\\')
            {
                c = fgetc(fp);
                if (c == 'n')
                {
                    token[i++] = '\n';
                    continue;
                }
                else if (c == 'r')
                {
                    token[i++] = '\r';
                    continue;
                }
                else if (c == '0')
                {
                    token[i++] = '\0';
                    continue;
                }
            }
            token[i++] = c;
        }
        token[i] = '\0';
        return token;
    }

    if (is_symbol(c))
    {
        int i = 0;
        token[i++] = c;
        token[i] = '\0';
        if (token[0] == '/')
        {
            int next_char = fgetc(fp);
            if (next_char == '/')
            {
                // read until \n
                while ((c = fgetc(fp)) != '\n' && c != EOF)
                    ;
                free(token);
                return get_token(fp);
            }
            ungetc(next_char, fp);
        }
        return token;
    }

    free(token);
    return NULL;
}

char parse_comparison_to_stack(FILE *file, char *char_set_to_compare)
{
    // first part
    char *firstPart = parse_until_charset(file, "<>!=");
    if (strcmp(firstPart, "=") != 0 && strcmp(firstPart, "<") != 0 && strcmp(firstPart, ">") != 0 && strcmp(firstPart, "!") != 0)
    {
        printf("Error: Expected =<>!\n");
        exit(1);
    }
    push_reg(REG_EAX);

    char *secondPart = parse_until_charset(file, char_set_to_compare);
    if (cmp_char_charset(secondPart[0], char_set_to_compare) == 0)
    {
        printf("Error: Expected %s\n", char_set_to_compare);
        exit(1);
    }
    pop_reg(REG_EBX);

    push_comparison_value_to_stack(REG_EBX, REG_EAX, firstPart);

    char c = secondPart[0];
    free(firstPart);
    free(secondPart);
    printf("returning %s\n", secondPart);
    return c;
}

// if condition false jump to to_jump
// parse until char_set_to_compare
void parse_comparison(FILE *file, char *to_jump, char *char_set_to_compare)
{
    int char_len = strlen(char_set_to_compare);
    char *new_charset = malloc(char_len + 3);
    strcpy(new_charset, char_set_to_compare);
    new_charset[char_len] = '&';
    new_charset[char_len + 1] = '|';
    new_charset[char_len + 2] = '\0';
    uint8_t *logic_gates = malloc(100);
    int logic_gates_count = 0;

    char c = parse_comparison_to_stack(file, new_charset);

    while (c == '&' || c == '|')
    {
        logic_gates[logic_gates_count] = c;
        logic_gates_count++;
        c = parse_comparison_to_stack(file, new_charset);
    }

    printf("logic gates count %d\n", logic_gates_count);
    for (int i = 0; i < logic_gates_count; i++)
    {
        if (logic_gates[i] == '&')
        {
            pop_eax();
            pop_ebx();

            andf(REG_EAX, REG_EBX);
            push_eax();
        }
        else if (logic_gates[i] == '|')
        {
            pop_eax();
            pop_ebx();

            orf(REG_EAX, REG_EBX);
            push_eax();
        }
    }

    pop_eax();
    mov_ebx(1);
    cmp_reg32(REG_EAX, REG_EBX);
    jump_if_not_equal(to_jump);
}

void parse_ifs(FILE *file)
{
    char *false_label = create_temp_label();
    char *end_label = create_temp_label();
    parse_comparison(file, false_label, "{"); // if comparison is false jump to false_label

    char *token = get_token(file);
    while (strcmp(token, "}") != 0)
    {
        parse_it(token, file);
        token = get_token(file);
    }
    jmp(end_label); // executed jmp to end

    create_label(false_label);

    char *else_token = get_token(file);
    if (strcmp(else_token, "elif") == 0)
    {
        parse_ifs(file);
    }
    else if (strcmp(else_token, "else") == 0)
    {
        parse_block(file);
    }
    else
    {
        go_back_x_char(strlen(else_token), file);
    }

    nop();
    create_label(end_label);

    free(token);
    free(false_label);
}

void parse_fors(FILE *file)
{
    char *token = get_token(file);
    parse_it(token, file); // for i = 0;

    char *comparison_label = create_temp_label();
    char *increment_label = create_temp_label();
    char *code_label = create_temp_label();
    char *endfor = create_temp_label();

    create_label(comparison_label);
    parse_comparison(file, endfor, ";");
    jmp(code_label);

    create_label(increment_label);
    token = get_token(file);
    parse_it(token, file); // i = i + 1;
    ret();

    create_label(code_label);
    parse_block(file);
    call(increment_label);
    jmp(comparison_label);

    create_label(endfor);

    free(endfor);
    free(comparison_label);
    free(code_label);
    free(increment_label);
}

void parse_while(FILE *file)
{
    char *temp_label_name = create_temp_label();
    char *temp_label_name_end = create_temp_label();

    create_label(temp_label_name);
    parse_comparison(file, temp_label_name_end, "{");

    parse_block(file);
    jmp(temp_label_name);
    create_label(temp_label_name_end);

    free(temp_label_name);
    free(temp_label_name_end);
}

char **included_files;
uint32_t number_of_files;

int parse_include(FILE *file)
{
    char *file_name = get_token(file);
    printf("Including file %s\n", file_name);

    for (int i = 0; i < number_of_files; i++)
    {
        if (strcmp(included_files[i], file_name) == 0)
        {
            printf("Error: File %s already included\n", file_name);
            exit(1);
        }
    }

    start_parsing(file_name);

    included_files = realloc(included_files, sizeof(char *) * (number_of_files + 1));
    included_files[number_of_files] = malloc(strlen(file_name) + 1);
    strcpy(included_files[number_of_files], file_name);
    number_of_files++;

    free(file_name);
    return 1;
}

// should free token;
int parse_it(char *token, FILE *file)
{
    if (strcmp(token, "include") == 0)
    {
        parse_include(file);
        free(token);
        return 1;
    }
    if (strcmp(token, "func") == 0)
    {
        parse_create_function(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "return") == 0)
    {
        parse_create_return(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "dd") == 0)
    {
        parse_create_int(file, DD);
        free(token);
        return 1;
    }

    if (strcmp(token, "dw") == 0)
    {
        parse_create_int(file, DW);
        free(token);
        return 1;
    }

    if (strcmp(token, "db") == 0)
    {
        parse_create_int(file, DB);
        free(token);
        return 1;
    }

    // conditionals
    if (strcmp(token, "if") == 0)
    {
        parse_ifs(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "for") == 0)
    {
        printf("\n\n\nFOR LOOP\n");
        parse_fors(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "while") == 0)
    {
        printf("\n\n\nWHILE LOOP\n");
        parse_while(file);
        free(token);
        return 1;
    }

    if (strcmp(token, "asm") == 0)
    {
        parse_asm_function(file);
        return 1;
    }

    if (strcmp(token, "struct") == 0)
    {
        parse_struct(file);
        return 1;
    }

    if (token[0] == '*')
    {
        parse_set_value_in_the_pointer_address(file);
        return 1;
    }

    // assumed that the token is a function call
    if (is_a_function(token))
    {
        printf("Calling function %s\n", token);
        parse_functions(file, token);
        free(token);
        return 1;
    }

    // by default set return value of the function to EAX
    if (parse_inside_functions(file, token, REG_EAX) == 1)
    {
        // does its thing inside the function
        return 1;
    }

    if (parse_struct_constructor(file, token) == 1)
    {
        return 1;
    }

    // assumed that the token is a variable
    // needs to be after all the initializers of variables
    int ident_question = does_var_exist(token);
    if (ident_question == 1)
    {
        // check if has a .
        char *next_token = get_token(file);
        if (next_token == NULL)
        {
            printf("Error: Expected a symbol after %s\n", token);
            exit(1);
        }
        if (strcmp(next_token, ".") == 0)
        {
            parse_set_struct_variables(file, token);
            free(next_token);
            return 1;
        }
        else
        {
            go_back_x_char(strlen(next_token), file);
        }

        parse_int_setter(file, token);
        return 1;
    }
    else
    {
        if (!is_symbol(token[0]))
        {
            printf("Error parse_it: Symbol %s not found\n", token);
            exit(EXIT_FAILURE);
        }
    }
    free(token);
    return 0;
}

char *BASE_COMPILE_FOLDER;
// If filename is "a/b/c/file.asm"
//   BASE_COMPILE_FOLDER will be "a/b/c/"
//   The function returns "file.asm"
//   And filenameOutput is updated to BASE_COMPILE_FOLDER + (its original value)

char *set_base_compile_folder(char *filename, char **filenameOutput)
{
    char *last_filename;
    const char *p = strrchr(filename, '/');
    if (p == NULL)
    {
        BASE_COMPILE_FOLDER = strdup("./");
        last_filename = strdup(filename);
    }
    else
    {
        size_t folder_len = p - filename + 1; // inclui a '/'
        BASE_COMPILE_FOLDER = strndup(filename, folder_len);
        last_filename = strdup(p + 1);
    }
    printf("BASE_COMPILE_FOLDER: %s\n", BASE_COMPILE_FOLDER);

    // Atualiza filenameOutput: prefixa BASE_COMPILE_FOLDER ao valor original.
    char *new_output = malloc(strlen(BASE_COMPILE_FOLDER) + strlen(*filenameOutput) + 1);
    if (new_output == NULL)
    {
        fprintf(stderr, "Erro na alocação de memória\n");
        exit(1);
    }
    strcpy(new_output, BASE_COMPILE_FOLDER);
    strcat(new_output, *filenameOutput);
    free(*filenameOutput);
    *filenameOutput = new_output;

    return last_filename;
}

void start_parsing(char *filename)
{
    char *finalFilename = malloc(strlen(BASE_COMPILE_FOLDER) + strlen(filename) + 1);
    strcpy(finalFilename, BASE_COMPILE_FOLDER);
    strcat(finalFilename, filename);

    printf("Parsing file %s\n\n\n\n\n\n\n\n", finalFilename);

    FILE *file = fopen(finalFilename, "r");
    if (file == NULL)
    {
        printf("Error: Could not open file %s\n", finalFilename);
        exit(1);
    }

    char *token;
    while ((token = get_token(file)) != NULL)
    {
        if (parse_it(token, file) == 0)
        {
        }
    }

    printf("\n\n\n\n\n\nFinished parsing file %s\n", finalFilename);

    fclose(file);
}

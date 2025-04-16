#include "structs.h"
#include "../parser.h"
#include "../int/parser_int.h"
#include "../../variables/variables.h"
#include "../../functions/bFunctions32/bFunctions32.h"
#include "../../functions/bFunctions8/bFunctions8.h"
#include "../../functions/functions.h"
#include "../parser_help.h"
#include "../../arithmetic/arithmetic.h"

// FOR STRUCT DECLARATION ONLY
/*
    struct myOwn{
        dd a;
        dd b;
    }
*/
typedef struct struct_variables
{
    char *name;
    int type;
    int offset;
    int size;
} struct_variables;

typedef struct struct_var
{
    char *name;
    struct_variables *vars;
    uint32_t vars_size;      // vars array size
    uint32_t vars_real_size; // vars size in memory 2 dds is 8
} struct_var;

struct_var *struct_vars;
uint32_t struct_vars_size;

void init_structs()
{
    struct_vars = NULL;
    struct_vars_size = 0;
}

void eat_semicolon(FILE *file)
{
    char *token = get_token(file);
    if (token[0] != ';')
    {
        fprintf(stderr, "Error: Expected ';' but got '%c'\n", token[0]);
        exit(1);
    }
    free(token);
}

void parse_struct(FILE *file)
{
    /*
    Exemplo:
    struct nome {
        dd a;
        dd b;
    }

    struct ja_existe;
    */
    // Obter o nome da struct
    char *name = get_token(file);
    printf("Struct name: %s\n", name);

    // Verificar se o nome da struct já está a ser utilizado
    for (uint32_t i = 0; i < struct_vars_size; i++)
    {
        if (strcmp(struct_vars[i].name, name) == 0)
        {
            fprintf(stderr, "Error: Struct name '%s' already exists.\n", name);
            exit(1);
        }
    }

    if (get_token(file)[0] != '{')
    {
        fprintf(stderr, "Error: Expected '{' after struct name\n");
        exit(1);
    }

    struct_variables *vars = NULL;
    uint32_t vars_size = 0;
    uint32_t vars_offset = 0;
    uint32_t vars_real_size = 0;

    // Loop para ler todas as variáveis
    while (1)
    {
        char *var_type = get_token(file);
        if (strcmp(var_type, "}") == 0)
        {
            break;
        }

        int type_enum;
        int size;

        // Verificar o tipo da variável
        if (strcmp(var_type, "dd") == 0)
        {
            printf("Var type: %s\n", var_type);
            type_enum = DD;
            size = 4;
            vars_real_size += size;
        }
        else if (strcmp(var_type, "db") == 0)
        {
            printf("Var type: %s\n", var_type);
            type_enum = DB;
            size = 1;
            vars_real_size += size;
        }
        else if (strcmp(var_type, "dw") == 0)
        {
            printf("Var type: %s\n", var_type);
            type_enum = DW;
            size = 2;
            vars_real_size += size;
        }
        else
        {
            fprintf(stderr, "Error: Unknown var type '%s'\n", var_type);
            exit(1);
        }

        // Obter o nome da variável
        char *var_name = get_token(file);

        // Verificar se o nome da variável já existe nesta struct
        for (uint32_t j = 0; j < vars_size; j++)
        {
            if (strcmp(vars[j].name, var_name) == 0)
            {
                fprintf(stderr, "Error: Variable name '%s' already exists in struct '%s'.\n", var_name, name);
                exit(1);
            }
        }

        // Adicionar a variável
        vars = realloc(vars, sizeof(struct_variables) * (vars_size + 1));
        vars[vars_size].type = type_enum;
        vars[vars_size].name = var_name;
        vars[vars_size].offset = vars_offset;
        vars[vars_size].size = size;
        vars_offset += size;
        vars_size++;

        eat_semicolon(file);
    }

    // Imprimir todas as variáveis
    for (uint32_t i = 0; i < vars_size; i++)
    {
        printf("Var name: %s, type: %d, offset: %d, size: %d\n",
               vars[i].name, vars[i].type, vars[i].offset, vars[i].size);
    }

    // Adicionar a nova struct à lista de structs
    struct_vars = realloc(struct_vars, sizeof(struct_var) * (struct_vars_size + 1));
    struct_vars[struct_vars_size].name = name;
    struct_vars[struct_vars_size].vars = vars;
    struct_vars[struct_vars_size].vars_size = vars_size;
    struct_vars[struct_vars_size].vars_real_size = vars_real_size;
    struct_vars_size++;

    // Imprimir todas as structs
    for (uint32_t i = 0; i < struct_vars_size; i++)
    {
        printf("Struct name: %s, vars size: %d, vars real size: %d\n",
               struct_vars[i].name, struct_vars[i].vars_size, struct_vars[i].vars_real_size);
    }
}

// STRUCT VAR CREATION
// myOwn s1(x41414141, x42424242);
char *get_random_struct_name()
{
    static int seeded = 0;
    if (!seeded)
    {
        srand(time(NULL));
        seeded = 1;
    }

    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int random_letters = 10;
    char *name = malloc(strlen("struct") + random_letters + 1);
    if (!name)
    {
        perror("Failed to allocate memory for struct name");
        exit(EXIT_FAILURE);
    }
    strcpy(name, "struct");
    int prefix_len = strlen("struct");
    for (int i = 0; i < random_letters; i++)
    {
        name[prefix_len + i] = charset[rand() % (sizeof(charset) - 1)];
    }
    name[prefix_len + random_letters] = '\0';
    return name;
}

void set_vars_from_type(char *var_name, int *current_offset, int index, int i)
{
    get_var(REG_EBX, var_name);
    if (struct_vars[index].vars[i].size == DD)
    {
        mov32_16_mi_r(REG_EBX, *current_offset, REG_EAX, 0);
        *current_offset += struct_vars[index].vars[i].size;
    }
    else if (struct_vars[index].vars[i].size == DW)
    {
        mov32_16_mi_r(REG_EBX, *current_offset, REG_EAX, 1);
        *current_offset += struct_vars[index].vars[i].size;
    }
    else if (struct_vars[index].vars[i].size == DB)
    {
        mov8_mi_r(REG_EBX, *current_offset, REG_EAX);
        *current_offset += struct_vars[index].vars[i].size;
    }
    else
    {
        fprintf(stderr, "Error: Unknown type %d\n", struct_vars[index].vars[i].size);
        exit(1);
    }
}

int set_struct_vars(FILE *file, char *var_name, int index, char *firstParam)
{
    if (strcmp(firstParam, "(") != 0)
    {
        fprintf(stderr, "Error: Expected '(' after struct name\n");
        exit(1);
    }
    int current_offset = 0;
    int i = 0;
    char *ret_from_parse = parse_until_charset(file, ",)");
    set_vars_from_type(var_name, &current_offset, index, i);
    while (ret_from_parse[0] != ')')
    {
        if (strcmp(ret_from_parse, ",") == 0)
        {
            free(ret_from_parse);
            ret_from_parse = parse_until_charset(file, ",)");
            i++;
            if (i >= struct_vars[index].vars_size)
            {
                fprintf(stderr, "Error: Too many parameters for struct %s\n", var_name);
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "Error: Expected ',' or ')'\n");
            exit(1);
        }

        set_vars_from_type(var_name, &current_offset, index, i);
    }
}

typedef struct
{
    char *var_name;
    int struct_index;
    char *var_scope;
} struct_var_type;

struct_var_type *struct_var_types;
uint32_t struct_var_types_size;
void init_struct_var_types()
{
    struct_var_types = NULL;
    struct_var_types_size = 0;
}

int parse_struct_constructor(FILE *file, char *token)
{
    // there are 2 vars becouse its a struct
    //  we have struct_name var pointing to the random name
    char *struct_name = get_token(file);

    for (uint32_t i = 0; i < struct_vars_size; i++)
    {
        if (strcmp(struct_vars[i].name, token) == 0)
        {
            create_var(struct_name, 4, 4, DD); // 4 bytes becouse addresses are 4

            // Create a random name for the struct
            char *random_name = get_random_struct_name();
            create_var(random_name, struct_vars[i].vars_real_size, struct_vars[i].vars_real_size, DD);

            set_var_with_reg(struct_name, REG_ESP);

            char *semi_colon = get_token(file);
            if (semi_colon[0] != ';')
            {
                // first param becouse ITS not a semicolon
                if (set_struct_vars(file, struct_name, i, semi_colon) == 0)
                {
                    fprintf(stderr, "Error: Expected ';' or '}' after struct name\n");
                    exit(1);
                }
            }
            free(semi_colon);

            // set the struct var type
            struct_var_types = realloc(struct_var_types, sizeof(struct_var_type) * (struct_var_types_size + 1));
            struct_var_types[struct_var_types_size].var_name = strdup(struct_name);
            struct_var_types[struct_var_types_size].struct_index = i;
            struct_var_types[struct_var_types_size].var_scope = strdup(get_current_scope());
            struct_var_types_size++;
            return 1;
        }
    }
    go_back_x_char(strlen(struct_name), file);
    return 0;
}

// set var
// s1.a = 10;
int parse_set_struct_variables(FILE *file, char *token)
{
    // token is struct var name and dot is gone
    char *offset_var = get_token(file);
    if (offset_var == NULL)
    {
        fprintf(stderr, "Error: Expected a variable name after '.'\n");
        exit(1);
    }
    // token -> s1
    // offset_var -> a

    for (uint32_t i = 0; i < struct_var_types_size; i++)
    {
        if (strcmp(struct_var_types[i].var_name, token) == 0 &&
            strcmp(struct_var_types[i].var_scope, get_current_scope()) == 0)

        {
            // get the struct index
            int struct_index = struct_var_types[i].struct_index;

            // check if the offset var is in the struct
            // Struct in itself
            for (uint32_t j = 0; j < struct_vars[struct_index].vars_size; j++)
            {
                if (strcmp(struct_vars[struct_index].vars[j].name, offset_var) == 0)
                {
                    char *equal = get_token(file);
                    if (equal[0] != '=')
                    {
                        fprintf(stderr, "Error: Expected '=' after struct variable name\n");
                        exit(1);
                    }
                    free(equal);

                    parse_until_charset(file, ";");
                    get_var(REG_EBX, token);

                    if (struct_vars[struct_index].vars[j].size == DD)
                    {
                        mov32_16_mi_r(REG_EBX, struct_vars[struct_index].vars[j].offset, REG_EAX, 0);
                    }
                    else if (struct_vars[struct_index].vars[j].size == DW)
                    {
                        mov32_16_mi_r(REG_EBX, struct_vars[struct_index].vars[j].offset, REG_EAX, 1);
                    }
                    else if (struct_vars[struct_index].vars[j].size == DB)
                    {
                        mov8_mi_r(REG_EBX, struct_vars[struct_index].vars[j].offset, REG_EAX);
                    }
                    else
                    {
                        fprintf(stderr, "Error: Unknown type %d\n", struct_vars[struct_index].vars[j].size);
                        exit(1);
                    }
                }
            }
        }
    }
}

// get var
// var = s1.a;
int parse_get_struct_variables(FILE *file, char *token, uint8_t reg)
{
    // token is struct var name and dot is gone
    char *offset_var = get_token(file);
    if (offset_var == NULL)
    {
        fprintf(stderr, "Error: Expected a variable name after '.'\n");
        exit(1);
    }
    // token -> s1
    // offset_var -> a

    for (uint32_t i = 0; i < struct_var_types_size; i++)
    {
        if (strcmp(struct_var_types[i].var_name, token) == 0)
        {
            // get the struct index
            int struct_index = struct_var_types[i].struct_index;

            // check if the offset var is in the struct
            // Struct in itself
            for (uint32_t j = 0; j < struct_vars[struct_index].vars_size; j++)
            {
                if (strcmp(struct_vars[struct_index].vars[j].name, offset_var) == 0)
                {
                    get_var(REG_EBX, token);
                    if (struct_vars[struct_index].vars[j].size == DD)
                    {
                        mov32_16_r_mi(reg, REG_EBX, struct_vars[struct_index].vars[j].offset, 0);
                    }
                    else if (struct_vars[struct_index].vars[j].size == DW)
                    {
                        mov32_16_r_mi(reg, REG_EBX, struct_vars[struct_index].vars[j].offset, 1);
                    }
                    else if (struct_vars[struct_index].vars[j].size == DB)
                    {
                        mov8_r_mi(reg, REG_EBX, struct_vars[struct_index].vars[j].offset);
                    }
                    else
                    {
                        fprintf(stderr, "Error: Unknown type %d\n", struct_vars[struct_index].vars[j].size);
                        exit(1);
                    }
                }
            }
        }
    }
}

// get addr var
// var = s1-a;
int parse_getADDR_struct_variables(FILE *file, char *token, uint8_t reg)
{
    // token is struct var name and dot is gone
    char *offset_var = get_token(file);
    if (offset_var == NULL)
    {
        fprintf(stderr, "Error: Expected a variable name after '.'\n");
        exit(1);
    }
    // token -> s1
    // offset_var -> a

    for (uint32_t i = 0; i < struct_var_types_size; i++)
    {
        if (strcmp(struct_var_types[i].var_name, token) == 0)
        {
            // get the struct index
            int struct_index = struct_var_types[i].struct_index;

            // check if the offset var is in the struct
            // Struct in itself
            for (uint32_t j = 0; j < struct_vars[struct_index].vars_size; j++)
            {
                if (strcmp(struct_vars[struct_index].vars[j].name, offset_var) == 0)
                {
                    get_var(REG_EBX, token);
                    add(REG_EBX, struct_vars[struct_index].vars[j].offset);
                }
            }
        }
    }
}
#include "structs.h"
#include "../parser.h"
#include "../int/parser_int.h"
#include "../../variables/variables.h"
#include "../../functions/bFunctions32/bFunctions32.h"
#include "../../functions/functions.h"

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

int parse_struct_contructors(FILE *file, char *token, int reg)
{
    for (uint32_t i = 0; i < struct_vars_size; i++)
    {
        if (strcmp(struct_vars[i].name, token) == 0)
        {
            char *name = get_random_struct_name();

            create_var(name, struct_vars[i].vars_real_size, struct_vars[i].vars_real_size, DD);
            mov32_16_r_r(REG_EAX, REG_ESP, 0);
            return 1;
        }
    }
    return 0;
}

int parse_struct_variables(FILE *file, char *token, int reg)
{
}
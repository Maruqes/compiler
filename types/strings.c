#include "strings.h"
#include "../functions/functions.h"

// they are strings wrote directlly on the executable binaries, thats not he ideia os this programming lagunage
// one day will be deleted :D

Constant_string *constant_strings = 0;
uint32_t constant_string_count = 0;

size_t data_size = 0;
void create_constant_string(char *var_name, char *var_value, uint32_t var_address)
{
    constant_strings = realloc(constant_strings, sizeof(Constant_string) * (constant_string_count + 1));
    if (constant_strings == NULL)
    {
        printf("Error: Could not allocate memory for constant_strings\n");
        exit(1);
    }
    constant_strings[constant_string_count].var_name = var_name;
    constant_strings[constant_string_count].var_value = var_value;
    constant_strings[constant_string_count].var_address = var_address;
    constant_string_count++;
    data_size += strlen(var_value) + 1; // ver o caralho do +1
    printf("Data size: %zu\n", data_size);
}

void write_all_string_constants(int __fd)
{
    for (size_t i = 0; i < constant_string_count; i++)
    {
        printf("Writing %s\n", constant_strings[i].var_value);
        ssize_t bytes_written = write(__fd, constant_strings[i].var_value, strlen(constant_strings[i].var_value) + 1);
        if (bytes_written == -1)
        {
            perror("Error writing string constant");
            exit(1);
        }
    }
}

uint32_t get_string_len(char *str)
{
    for (int i = 0; i < constant_string_count; i++)
    {
        if (strcmp(constant_strings[i].var_value, str) == 0)
        {
            return strlen(constant_strings[i].var_value) + 1; // +1 for null terminator
        }
    }
}

Constant_string *constant_strings_before = 0;
uint32_t constant_string_count_before = 0;
void create_constant_string_before(char *var_name, char *var_value)
{
    // Check if the variable name already exists
    for (size_t i = 0; i < constant_string_count_before; i++)
    {
        if (strcmp(constant_strings_before[i].var_name, var_name) == 0)
        {
            printf("Variable name already exists: %s\n", var_name);
            printf("Variable name already exists: %s\n", var_name);
            exit(1);
        }
    }

    // create copies of varname and str
    char *varname_copy = strdup(var_name);
    char *str_copy = strdup(var_value);
    if (varname_copy == NULL || str_copy == NULL)
    {
        printf("Error: Could not allocate memory for varname_copy or str_copy\n");
        exit(1);
    }

    constant_strings_before = realloc(constant_strings_before, sizeof(Constant_string) * (constant_string_count_before + 1));
    if (constant_strings_before == NULL)
    {
        printf("Error: Could not allocate memory for constant_strings_before\n");
        exit(1);
    }
    constant_strings_before[constant_string_count_before].var_name = varname_copy;
    constant_strings_before[constant_string_count_before].var_value = str_copy;
    constant_string_count_before++;
}

void fix_before_strings(uint32_t var_address)
{
    for (size_t i = 0; i < constant_string_count_before; i++)
    {
        create_constant_string(constant_strings_before[i].var_name, constant_strings_before[i].var_value, var_address + data_size);
    }
}

int is_a_string_beforeconstant(char *token)
{
    for (size_t i = 0; i < constant_string_count_before; i++)
    {
        if (strcmp(token, constant_strings_before[0].var_name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void free_strings()
{

    for (size_t i = 0; i < constant_string_count_before; i++)
    {
        free(constant_strings_before[i].var_value);
        free(constant_strings_before[i].var_name);
    }
    free(constant_strings);
    free(constant_strings_before);
}
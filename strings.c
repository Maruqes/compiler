#include "strings.h"
#include "functions.h"

Constant_string *constant_strings = 0;
uint32_t constant_string_count = 0;

void create_constant_string(char *var_name, char *var_value, uint32_t var_address)
{
    constant_strings = realloc(constant_strings, sizeof(Constant_string) * (constant_string_count + 1));
    constant_strings[constant_string_count].var_name = var_name;
    constant_strings[constant_string_count].var_value = var_value;
    constant_strings[constant_string_count].var_address = var_address;
    constant_string_count++;
    custom_code_size += strlen(var_value);
}

void write_all_string_constants(int __fd)
{
    for (size_t i = 0; i < constant_string_count; i++)
    {
        printf("Writing %s\n", constant_strings[i].var_value);
        write(__fd, constant_strings[i].var_value, strlen(constant_strings[i].var_value));
    }
}
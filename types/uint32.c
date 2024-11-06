#include "uint32.h"
#include "../functions/functions.h"
#include "strings.h"

Constant_uint32 *constant_uint32s = 0;
uint32_t constant_uint32_count = 0;

void create_constant_uint32(char *var_name, uint32_t var_value, uint32_t var_address)
{
    constant_uint32s = realloc(constant_uint32s, sizeof(Constant_uint32) * (constant_uint32_count + 1));
    if (constant_uint32s == NULL)
    {
        printf("Error: Could not allocate memory for constant_uint32s\n");
        exit(1);
    }
    constant_uint32s[constant_uint32_count].var_name = var_name;
    constant_uint32s[constant_uint32_count].var_value = var_value;
    constant_uint32s[constant_uint32_count].var_address = var_address;
    constant_uint32_count++;
    data_size += 4;
}

void write_all_uint32_constants(int __fd)
{
    for (size_t i = 0; i < constant_uint32_count; i++)
    {
        ssize_t bytes_written = write(__fd, &constant_uint32s[i].var_value, sizeof(constant_uint32s[i].var_value));
        if (bytes_written == -1)
        {
            perror("Error writing uint32 constant");
            exit(1);
        }
    }
}

void free_uint32s()
{
    free(constant_uint32s);
}

Constant_uint32 *constant_uint32s_before = 0;
uint32_t constant_uint32_count_before = 0;

void create_constant_uint32_before(char *var_name, uint32_t var_value)
{
    constant_uint32s_before = realloc(constant_uint32s_before, sizeof(Constant_uint32) * (constant_uint32_count_before + 1));
    if (constant_uint32s_before == NULL)
    {
        printf("Error: Could not allocate memory for constant_uint32s_before\n");
        exit(1);
    }
    constant_uint32s_before[constant_uint32_count_before].var_name = var_name;
    constant_uint32s_before[constant_uint32_count_before].var_value = var_value;
    constant_uint32_count_before++;
}

void fix_before_uint32s(uint32_t var_address)
{
    for (size_t i = 0; i < constant_uint32_count_before; i++)
    {
        create_constant_uint32(constant_uint32s_before[i].var_name, constant_uint32s_before[i].var_value, var_address);
    }
}

int is_a_uint32_beforeconstant(char *token)
{
    for (size_t i = 0; i < constant_uint32_count_before; i++)
    {
        if (strcmp(token, constant_uint32s_before[0].var_name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// ARRAY
Constant_uint32_arr *constant_uint32_arrs;
uint32_t constant_uint32_arr_count = 0;

void create_constant_uint32_arr(char *var_name, uint32_t *var_values, uint32_t var_address, uint32_t number_of_values)
{

    constant_uint32_arrs = realloc(constant_uint32_arrs, sizeof(Constant_uint32_arr) * (constant_uint32_arr_count + 1));
    if (constant_uint32_arrs == NULL)
    {
        printf("Error: Could not allocate memory for constant_uint32_arrs\n");
        exit(1);
    }
    constant_uint32_arrs[constant_uint32_arr_count].var_name = var_name;
    constant_uint32_arrs[constant_uint32_arr_count].var_values = malloc(sizeof(uint32_t) * number_of_values);
    memcpy(constant_uint32_arrs[constant_uint32_arr_count].var_values, var_values, sizeof(uint32_t) * number_of_values);
    constant_uint32_arrs[constant_uint32_arr_count].var_address = var_address;
    constant_uint32_arrs[constant_uint32_arr_count].number_of_values = number_of_values;
    constant_uint32_arr_count++;
    data_size += 4 * number_of_values;
}

void write_all_uint32_arr_constants(int __fd)
{
    for (size_t i = 0; i < constant_uint32_arr_count; i++)
    {
        ssize_t bytes_written = write(__fd, constant_uint32_arrs[i].var_values, sizeof(uint32_t) * constant_uint32_arrs[i].number_of_values);
        if (bytes_written == -1)
        {
            perror("Error writing uint32 array constant");
            exit(1);
        }
    }
}

Constant_uint32_arr *constant_uint32_arrs_before;
uint32_t constant_uint32_arr_count_before = 0;

void create_constant_uint32_arr_before(char *var_name, uint32_t *var_values, uint32_t number_of_values)
{
    constant_uint32_arrs_before = realloc(constant_uint32_arrs_before, sizeof(Constant_uint32_arr) * (constant_uint32_arr_count_before + 1));
    if (constant_uint32_arrs_before == NULL)
    {
        printf("Error: Could not allocate memory for constant_uint32_arrs_before\n");
        exit(1);
    }
    constant_uint32_arrs_before[constant_uint32_arr_count_before].var_name = var_name;
    constant_uint32_arrs_before[constant_uint32_arr_count_before].var_values = malloc(sizeof(uint32_t) * number_of_values);
    memcpy(constant_uint32_arrs_before[constant_uint32_arr_count_before].var_values, var_values, sizeof(uint32_t) * number_of_values);
    constant_uint32_arrs_before[constant_uint32_arr_count_before].number_of_values = number_of_values;
    constant_uint32_arr_count_before++;
}

void fix_before_uint32_arrs(uint32_t var_address)
{
    for (size_t i = 0; i < constant_uint32_arr_count_before; i++)
    {
        create_constant_uint32_arr(constant_uint32_arrs_before[i].var_name, constant_uint32_arrs_before[i].var_values, var_address, constant_uint32_arrs_before[i].number_of_values);
    }
}

int is_a_uint32_arr_beforeconstant(char *token)
{
    for (size_t i = 0; i < constant_uint32_arr_count_before; i++)
    {
        if (strcmp(token, constant_uint32_arrs_before[0].var_name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

void free_uint32_arrs()
{
    for (size_t i = 0; i < constant_uint32_arr_count; i++)
    {
        free(constant_uint32_arrs[i].var_values);
    }
    free(constant_uint32_arrs);

    for (size_t i = 0; i < constant_uint32_arr_count_before; i++)
    {
        free(constant_uint32_arrs_before[i].var_values);
        free(constant_uint32_arrs_before[i].var_name);
    }
    free(constant_uint32_arrs_before);

    for (size_t i = 0; i < constant_uint32_count_before; i++)
    {
        free(constant_uint32s_before->var_name);
    }
    free(constant_uint32s_before);
}

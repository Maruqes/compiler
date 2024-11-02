#include "uint32.h"
#include "../functions/functions.h"
#include "strings.h"

Constant_uint32 *constant_uint32s = 0;
uint32_t constant_uint32_count = 0;

void create_constant_uint32(char *var_name, uint32_t var_value, uint32_t var_address)
{
    constant_uint32s = realloc(constant_uint32s, sizeof(Constant_uint32) * (constant_uint32_count + 1));
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
        write(__fd, &constant_uint32s[i].var_value, sizeof(constant_uint32s[i].var_value));
    }
}

Constant_uint32 *constant_uint32s_before = 0;
uint32_t constant_uint32_count_before = 0;

void create_constant_uint32_before(char *var_name, uint32_t var_value)
{
    constant_uint32s_before = realloc(constant_uint32s_before, sizeof(Constant_uint32) * (constant_uint32_count_before + 1));
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
        write(__fd, constant_uint32_arrs[i].var_values, sizeof(uint32_t) * constant_uint32_arrs[i].number_of_values);
    }
}

Constant_uint32_arr *constant_uint32_arrs_before;
uint32_t constant_uint32_arr_count_before = 0;

void create_constant_uint32_arr_before(char *var_name, uint32_t *var_values, uint32_t number_of_values)
{
    constant_uint32_arrs_before = realloc(constant_uint32_arrs_before, sizeof(Constant_uint32_arr) * (constant_uint32_arr_count_before + 1));
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
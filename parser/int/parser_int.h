#ifndef PARSER_INT_H
#define PARSER_INT_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void parse_int_setter(FILE *file, char *token);
void parse_create_int(FILE *file, int type);
void parse_set_value_in_the_pointer_address(FILE *file);
void parse_data_types(FILE *file, char *token, uint8_t reg);
void parse_int_array_value_setter(FILE *file, char *arr_var_name);
void parse_after_equal(FILE *file);

typedef enum Value
{
    DB = 1,
    DW = 2,
    DD = 4,
} Value;



uint32_t get_type_length(int valueType);
int check_type(char *type);

#endif
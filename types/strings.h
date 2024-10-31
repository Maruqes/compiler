#ifndef STRINGS_H
#define STRINGS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct Constant_string
{
    char *var_name;
    char *var_value;
    uint32_t var_address;
} Constant_string;

extern Constant_string *constant_strings;
extern uint32_t constant_string_count;
extern size_t data_size;

void create_constant_string(char *var_name, char *var_value, uint32_t var_address);
void write_all_string_constants(int __fd);
void fixup_addresses();
uint32_t get_string_len(char *str);

void create_constant_string_before(char *var_name, char *var_value);
void fix_before_strings(uint32_t var_address);

#endif
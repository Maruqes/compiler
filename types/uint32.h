#ifndef UINT32_H
#define UINT32_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct Constant_uint32
{
    char *var_name;
    uint32_t var_value;
    uint32_t var_address;
} Constant_uint32;

extern Constant_uint32 *constant_uint32s;
extern uint32_t constant_uint32_count;

void create_constant_uint32(char *var_name, uint32_t var_value, uint32_t var_address);
void write_all_uint32_constants(int __fd);

extern Constant_uint32 *constant_uint32s_before;
extern uint32_t constant_uint32_count_before;

void create_constant_uint32_before(char *var_name, uint32_t var_value);
void fix_before_uint32s(uint32_t var_address);

int is_a_uint32_beforeconstant(char *token);

#endif
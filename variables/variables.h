#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct Variable
{
    char *symbol;
    uint32_t size;
    uint32_t offset;
    char *scope;
} Variable;

void create_var(char *symbol, uint32_t size);
void free_variables_array();
void create_new_stack();
void restore_stack();

void set_var_with_reg(char *symbol, uint8_t reg);

void set_var(char *symbol, uint32_t value);
void get_var(uint8_t reg, char *symbol);
int does_var_exist(char *symbol);
Variable return_var_struct(char *symbol);

#endif // VARIABLES_H
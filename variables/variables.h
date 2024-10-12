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
} Variable;

void create_var(char *symbol, uint32_t size);
void create_new_stack();
void restore_stack();

void mov_reg_reg_offset(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void set_var(char *symbol, uint32_t value);
void get_var(uint8_t reg, char *symbol);

#endif // VARIABLES_H
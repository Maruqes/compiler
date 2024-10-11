#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// arithmetic
void add(uint8_t reg, uint32_t value);
void add_reg32(uint8_t reg1, uint8_t reg2);

void sub(uint8_t reg, uint32_t value);
void sub_reg32(uint8_t reg1, uint8_t reg2);

void mul_reg32(uint8_t reg1, uint8_t reg2);
#endif // ARITHMETIC_H
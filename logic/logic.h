#ifndef LOGIC_H
#define LOGIC_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void andf(uint8_t reg1, uint8_t reg2);
void orf(uint8_t reg1, uint8_t reg2);
void xorf(uint8_t reg1, uint8_t reg2);
void not(uint8_t reg);

#endif // LOGIC_H
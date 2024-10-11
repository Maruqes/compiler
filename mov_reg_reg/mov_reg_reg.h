#ifndef MOV_REG_REG_H
#define MOV_REG_REG_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
void mov_reg32_reg32(uint8_t reg1, uint8_t reg2);
void mov_reg16_reg16(uint8_t reg1, uint8_t reg2);
void mov_reg8_reg8(uint8_t reg1, uint8_t reg2);
#endif // MOV_REG_REG_H
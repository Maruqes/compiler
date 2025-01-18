#ifndef BFUNCTIONS32_H
#define BFUNCTIONS32_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void mov32_r_i(uint8_t reg_code, uint32_t value);
void mov32_r_m(uint8_t reg, uint8_t mem_reg);
void mov32_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void mov32_r_mr(uint8_t reg_base, uint8_t reg2, uint8_t reg3);
void mov32_r_r(uint8_t reg1, uint8_t reg2);
void mov32_m_i(uint8_t reg1, uint32_t value);
void mov32_m_r(uint8_t reg1, uint8_t reg2);
void mov32_mi_i(uint8_t reg, int32_t offset, uint32_t value);
void mov32_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2);
void mov32_mr_i(uint8_t reg, uint8_t reg2, uint32_t value);
void mov32_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3);
#endif // BFUNCTIONS32_H
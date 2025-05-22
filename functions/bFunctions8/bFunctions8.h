#ifndef BFUNCTIONS8_H
#define BFUNCTIONS8_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

//define al bl regs... ah bl
#define AL 0
#define CL 1
#define DL 2
#define BL 3
#define AH 4
#define CH 5
#define DH 6
#define BH 7

void mov8_r_i(uint8_t reg_code, uint8_t value);
void mov8_r_m(uint8_t reg, uint8_t mem_reg);
void mov8_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void mov8_r_mr(uint8_t reg_base, uint8_t reg2, uint8_t reg3);
void mov8_r_r(uint8_t reg1, uint8_t reg2);
void mov8_m_i(uint8_t reg1, uint8_t value);
void mov8_m_r(uint8_t reg1, uint8_t reg2);
void mov8_mi_i(uint8_t reg, int32_t offset, uint32_t value);
void mov8_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2);
void mov8_mr_i(uint8_t reg, uint8_t reg2, uint32_t value);
void mov8_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3);
#endif // BFUNCTIONS8_H
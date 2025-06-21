#ifndef BFUNCTIONS8_H
#define BFUNCTIONS8_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// define al bl regs... ah bl
#define REG_AL 0
#define REG_CL 1
#define REG_DL 2
#define REG_BL 3
#define REG_AH 4
#define REG_CH 5
#define REG_DH 6
#define REG_BH 7

void mov8_r_i(uint8_t reg_code, uint8_t value);
void mov8_r_m(uint8_t reg, uint8_t mem_reg);
void mov8_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void mov8_r_mr(uint8_t reg_base, uint8_t reg2, uint8_t reg3);
void mov8_r_r(uint8_t reg1, uint8_t reg2);
void mov8_m_i(uint8_t reg1, uint8_t value);
void mov8_m_r(uint8_t reg1, uint8_t reg2);
void mov8_mi_i(uint8_t reg, int32_t offset, uint8_t value);
void mov8_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2);
void mov8_mr_i(uint8_t reg, uint8_t reg2, uint8_t value);
void mov8_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3);

// Test functions declarations
void funcao_teste_mov8_r_i(void);
void funcao_teste_mov8_r_m(void);
void funcao_teste_mov8_r_mi(void);
void funcao_teste_mov8_r_mr(void);
void funcao_teste_mov8_r_r(void);
void funcao_teste_mov8_m_i(void);
void funcao_teste_mov8_m_r(void);
void funcao_teste_mov8_mi_i(void);
void funcao_teste_mov8_mi_r(void);
void funcao_teste_mov8_mr_i(void);
void funcao_teste_mov8_mr_r(void);

#endif // BFUNCTIONS8_H
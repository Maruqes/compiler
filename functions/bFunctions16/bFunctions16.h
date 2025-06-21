#ifndef BFUNCTIONS16_H
#define BFUNCTIONS16_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define REG_AX 0
#define REG_CX 1
#define REG_DX 2
#define REG_BX 3
#define REG_SP 4
#define REG_BP 5
#define REG_SI 6
#define REG_DI 7

void mov16_r_i(uint8_t reg_code, uint16_t value);
void mov16_r_m(uint8_t reg, uint8_t mem_reg);
void mov16_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void mov16_r_mr(uint8_t reg_base, uint8_t reg2, uint8_t reg3);
void mov16_r_r(uint8_t reg1, uint8_t reg2);
void mov16_m_i(uint8_t reg1, uint16_t value);
void mov16_m_r(uint8_t reg1, uint8_t reg2);
void mov16_mi_i(uint8_t reg, int32_t offset, uint16_t value);
void mov16_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2);
void mov16_mr_i(uint8_t reg, uint8_t reg2, uint16_t value);
void mov16_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3);

// Test functions declarations
void funcao_teste_mov16_r_i(void);
void funcao_teste_mov16_r_m(void);
void funcao_teste_mov16_r_mi(void);
void funcao_teste_mov16_r_mr(void);
void funcao_teste_mov16_r_r(void);
void funcao_teste_mov16_m_i(void);
void funcao_teste_mov16_m_r(void);
void funcao_teste_mov16_mi_i(void);
void funcao_teste_mov16_mi_r(void);
void funcao_teste_mov16_mr_i(void);
void funcao_teste_mov16_mr_r(void);

#endif // BFUNCTIONS16_H

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

// Compare functions declarations
void cmp8_r_r(uint8_t reg1, uint8_t reg2);
void cmp8_r_i(uint8_t reg1, uint8_t imm8);
void cmp8_r_m(uint8_t reg1, uint8_t reg2);
void cmp8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);

// Shift functions declarations
void lshfit8(uint8_t reg, uint8_t imm);
void rshfit8(uint8_t reg, uint8_t imm);
void lshfit8_reg(uint8_t reg1);
void rshfit8_reg(uint8_t reg1);

// AND functions declarations
void and8_r_r(uint8_t reg1, uint8_t reg2);
void and8_r_i(uint8_t reg, uint8_t imm8);
void and8_r_m(uint8_t reg1, uint8_t reg2);
void and8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void and8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// OR functions declarations
void or8_r_r(uint8_t reg1, uint8_t reg2);
void or8_r_i(uint8_t reg, uint8_t imm8);
void or8_r_m(uint8_t reg1, uint8_t reg2);
void or8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void or8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// NOT functions declarations
void not8_r(uint8_t reg);
void not8_m(uint8_t reg);
void not8_mi(uint8_t reg, uint32_t offset);
void not8_mr(uint8_t reg_base, uint8_t reg_index);

// XOR functions declarations
void xor8_r_r(uint8_t reg1, uint8_t reg2);
void xor8_r_i(uint8_t reg, uint8_t imm8);
void xor8_r_m(uint8_t reg1, uint8_t reg2);
void xor8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void xor8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// Arithmetic functions declarations
void sum8_r_r(uint8_t reg1, uint8_t reg2);
void sum8_r_i(uint8_t reg1, uint8_t imm8);
void sum8_r_m(uint8_t reg1, uint8_t reg2);
void sum8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void sum8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void sub8_r_r(uint8_t reg1, uint8_t reg2);
void sub8_r_i(uint8_t reg1, uint8_t imm8);
void sub8_r_m(uint8_t reg1, uint8_t reg2);
void sub8_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void sub8_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void mul8_r(uint8_t reg2);
void mul8_m(uint8_t reg2);
void mul8_mi(uint8_t reg2, uint32_t offset);
void mul8_mr(uint8_t reg2, uint8_t reg3);

void div8_r(uint8_t reg1);
void div8_m(uint8_t reg1);
void div8_mi(uint8_t reg2, uint32_t offset);
void div8_mr(uint8_t reg2, uint8_t reg3);

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

// Compare test functions declarations
void funcao_teste_cmp8_r_r(void);
void funcao_teste_cmp8_r_i(void);
void funcao_teste_cmp8_r_m(void);
void funcao_teste_cmp8_r_mi(void);

// Shift test functions declarations
void funcao_teste_lshfit8(void);
void funcao_teste_rshfit8(void);
void funcao_teste_lshfit8_reg(void);
void funcao_teste_rshfit8_reg(void);

// AND test functions declarations
void funcao_teste_and8_r_r(void);
void funcao_teste_and8_r_i(void);
void funcao_teste_and8_r_m(void);
void funcao_teste_and8_r_mi(void);
void funcao_teste_and8_r_mr(void);

// OR test functions declarations
void funcao_teste_or8_r_r(void);
void funcao_teste_or8_r_i(void);
void funcao_teste_or8_r_m(void);
void funcao_teste_or8_r_mi(void);
void funcao_teste_or8_r_mr(void);

// NOT test functions declarations
void funcao_teste_not8_r(void);
void funcao_teste_not8_m(void);
void funcao_teste_not8_mi(void);
void funcao_teste_not8_mr(void);

// XOR test functions declarations
void funcao_teste_xor8_r_r(void);
void funcao_teste_xor8_r_i(void);
void funcao_teste_xor8_r_m(void);
void funcao_teste_xor8_r_mi(void);
void funcao_teste_xor8_r_mr(void);

// Arithmetic test functions declarations
// SUM test functions
void funcao_teste_sum8_r_r(void);
void funcao_teste_sum8_r_i(void);
void funcao_teste_sum8_r_m(void);
void funcao_teste_sum8_r_mi(void);
void funcao_teste_sum8_r_mr(void);

// SUB test functions
void funcao_teste_sub8_r_r(void);
void funcao_teste_sub8_r_i(void);
void funcao_teste_sub8_r_m(void);
void funcao_teste_sub8_r_mi(void);
void funcao_teste_sub8_r_mr(void);

// MUL test functions
void funcao_teste_mul8_r(void);
void funcao_teste_mul8_m(void);
void funcao_teste_mul8_mi(void);
void funcao_teste_mul8_mr(void);

// DIV test functions
void funcao_teste_div8_r(void);
void funcao_teste_div8_m(void);
void funcao_teste_div8_mi(void);
void funcao_teste_div8_mr(void);

#endif // BFUNCTIONS8_H
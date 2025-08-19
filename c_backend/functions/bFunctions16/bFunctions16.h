#ifndef BFUNCTIONS16_H
#define BFUNCTIONS16_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "../bFunctions64/bFunctions64.h"

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

// Compare functions
void cmp16_r_r(uint8_t reg1, uint8_t reg2);
void cmp16_r_i(uint8_t reg1, uint16_t imm16);
void cmp16_r_m(uint8_t reg1, uint8_t reg2);
void cmp16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);

// Shift functions
void lshfit16(uint8_t reg, uint8_t imm);
void rshfit16(uint8_t reg, uint8_t imm);
void lshfit16_reg(uint8_t reg1);
void rshfit16_reg(uint8_t reg1);

// Logical AND operations
void and16_r_r(uint8_t reg1, uint8_t reg2);
void and16_r_i(uint8_t reg, uint16_t imm16);
void and16_r_m(uint8_t reg1, uint8_t reg2);
void and16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void and16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// Logical OR operations
void or16_r_r(uint8_t reg1, uint8_t reg2);
void or16_r_i(uint8_t reg, uint16_t imm16);
void or16_r_m(uint8_t reg1, uint8_t reg2);
void or16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void or16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// Logical XOR operations
void xor16_r_r(uint8_t reg1, uint8_t reg2);
void xor16_r_i(uint8_t reg, uint16_t imm16);
void xor16_r_m(uint8_t reg1, uint8_t reg2);
void xor16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void xor16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// Logical NOT operations
void not16_r(uint8_t reg);
void not16_m(uint8_t reg);
void not16_mi(uint8_t reg, uint32_t offset);
void not16_mr(uint8_t reg_base, uint8_t reg_index);

// INC 16-bit operations
void inc16_r(uint8_t reg);
void inc16_m(uint8_t reg);
void inc16_mi(uint8_t reg, uint32_t offset);
void inc16_mr(uint8_t reg_base, uint8_t reg_index);

// DEC 16-bit operations
void dec16_r(uint8_t reg);
void dec16_m(uint8_t reg);
void dec16_mi(uint8_t reg, uint32_t offset);
void dec16_mr(uint8_t reg_base, uint8_t reg_index);

// Arithmetic operations - sum, sub, mul, div
void sum16_r_r(uint8_t reg1, uint8_t reg2);
void sum16_r_i(uint8_t reg1, uint16_t imm16);
void sum16_r_m(uint8_t reg1, uint8_t reg2);
void sum16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void sum16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void sub16_r_r(uint8_t reg1, uint8_t reg2);
void sub16_r_i(uint8_t reg1, uint16_t imm16);
void sub16_r_m(uint8_t reg1, uint8_t reg2);
void sub16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void sub16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void mul16_r_r(uint8_t reg1, uint8_t reg2);
void mul16_r_i(uint8_t reg1, uint16_t imm16);
void mul16_r_m(uint8_t reg1, uint8_t reg2);
void mul16_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void mul16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void div16_r(uint8_t reg1);
void div16_m(uint8_t reg1);
void div16_mi(uint8_t reg2, uint32_t offset);
void div16_mr(uint8_t reg2, uint8_t reg3);

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

// Compare test functions declarations
void funcao_teste_cmp16_r_r(void);
void funcao_teste_cmp16_r_i(void);
void funcao_teste_cmp16_r_m(void);
void funcao_teste_cmp16_r_mi(void);

// shift op
void funcao_teste_lshfit16(void);
void funcao_teste_rshfit16(void);
void funcao_teste_lshfit16_reg(void);
void funcao_teste_rshfit16_reg(void);

void funcao_teste_and16_r_r(void);
void funcao_teste_and16_r_i(void);
void funcao_teste_and16_r_m(void);
void funcao_teste_and16_r_mi(void);
void funcao_teste_and16_r_mr(void);

// OR test functions declarations
void funcao_teste_or16_r_r(void);
void funcao_teste_or16_r_i(void);
void funcao_teste_or16_r_m(void);
void funcao_teste_or16_r_mi(void);
void funcao_teste_or16_r_mr(void);

// XOR test functions declarations
void funcao_teste_xor16_r_r(void);
void funcao_teste_xor16_r_i(void);
void funcao_teste_xor16_r_m(void);
void funcao_teste_xor16_r_mi(void);
void funcao_teste_xor16_r_mr(void);

// NOT test functions declarations
void funcao_teste_not16_r(void);
void funcao_teste_not16_m(void);
void funcao_teste_not16_mi(void);
void funcao_teste_not16_mr(void);

// Arithmetic test functions declarations (SUM, SUB, MUL, DIV)
// SUM 16-bit tests
void funcao_teste_sum16_r_r(void);
void funcao_teste_sum16_r_i(void);
void funcao_teste_sum16_r_m(void);
void funcao_teste_sum16_r_mi(void);
void funcao_teste_sum16_r_mr(void);

// SUB 16-bit tests
void funcao_teste_sub16_r_r(void);
void funcao_teste_sub16_r_i(void);
void funcao_teste_sub16_r_m(void);
void funcao_teste_sub16_r_mi(void);
void funcao_teste_sub16_r_mr(void);

// MUL 16-bit tests
void funcao_teste_mul16_r_r(void);
void funcao_teste_mul16_r_i(void);
void funcao_teste_mul16_r_m(void);
void funcao_teste_mul16_r_mi(void);
void funcao_teste_mul16_r_mr(void);

// DIV 16-bit tests
void funcao_teste_div16_r(void);
void funcao_teste_div16_m(void);
void funcao_teste_div16_mi(void);
void funcao_teste_div16_mr(void);

#endif // BFUNCTIONS16_H

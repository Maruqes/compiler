#ifndef BFUNCTIONS32_H
#define BFUNCTIONS32_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include "../bFunctions64/bFunctions64.h"

#define REG_EAX 0x0 // 32-bit register defs kept for compatibility
#define REG_ECX 0x1
#define REG_EDX 0x2
#define REG_EBX 0x3
#define REG_ESP 0x4
#define REG_EBP 0x5
#define REG_ESI 0x6
#define REG_EDI 0x7
#define REG_R8D 0x8
#define REG_R9D 0x9
#define REG_R10D 0xA
#define REG_R11D 0xB
#define REG_R12D 0xC
#define REG_R13D 0xD
#define REG_R14D 0xE
#define REG_R15D 0xF

#define XMM0 0x0
#define XMM1 0x1
#define XMM2 0x2
#define XMM3 0x3
#define XMM4 0x4
#define XMM5 0x5
#define XMM6 0x6
#define XMM7 0x7

void cant_use_rx(uint8_t reg[], size_t size);

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



// Arithmetic operations
void sum32_r_r(uint8_t reg1, uint8_t reg2);
void sum32_r_i(uint8_t reg1, uint32_t imm32);
void sum32_r_m(uint8_t reg1, uint8_t reg2);
void sum32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void sum32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void sub32_r_r(uint8_t reg1, uint8_t reg2);
void sub32_r_i(uint8_t reg1, uint32_t imm32);
void sub32_r_m(uint8_t reg1, uint8_t reg2);
void sub32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void sub32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void mul32_r_r(uint8_t reg1, uint8_t reg2);
void mul32_r_i(uint8_t reg1, uint32_t imm32);
void mul32_r_m(uint8_t reg1, uint8_t reg2);
void mul32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void mul32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

void div32_r(uint8_t reg1);
void div32_m(uint8_t reg1);
void div32_mi(uint8_t reg2, uint32_t offset);
void div32_mr(uint8_t reg2, uint8_t reg3);

// utils
void cmp32_r_r(uint8_t reg1, uint8_t reg2);
void cmp32_r_i(uint8_t reg1, uint32_t imm32);
void cmp32_r_m(uint8_t reg1, uint8_t reg2);
void cmp32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void rshfit32(uint8_t reg, uint8_t imm);
void lshfit32(uint8_t reg, uint8_t imm);
void lshfit32_reg(uint8_t reg1);
void rshfit32_reg(uint8_t reg1);
void and32_r_r(uint8_t reg1, uint8_t reg2);
void and32_r_i(uint8_t reg, uint32_t imm32);
void and32_r_m(uint8_t reg1, uint8_t reg2);
void and32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void and32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);
void or32_r_r(uint8_t reg1, uint8_t reg2);
void or32_r_i(uint8_t reg, uint32_t imm32);
void or32_r_m(uint8_t reg1, uint8_t reg2);
void or32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void or32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// XOR 32-bit operations
void xor32_r_r(uint8_t reg1, uint8_t reg2);
void xor32_r_i(uint8_t reg, uint32_t imm32);
void xor32_r_m(uint8_t reg1, uint8_t reg2);
void xor32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void xor32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// NOT 32-bit operations
void not32_r(uint8_t reg);
void not32_m(uint8_t reg);
void not32_mi(uint8_t reg, uint32_t offset);
void not32_mr(uint8_t reg_base, uint8_t reg_index);

// INC 32-bit operations
void inc32_r(uint8_t reg);
void inc32_m(uint8_t reg);
void inc32_mi(uint8_t reg, uint32_t offset);
void inc32_mr(uint8_t reg_base, uint8_t reg_index);

// DEC 32-bit operations
void dec32_r(uint8_t reg);
void dec32_m(uint8_t reg);
void dec32_mi(uint8_t reg, uint32_t offset);
void dec32_mr(uint8_t reg_base, uint8_t reg_index);

// Test functions
void funcao_teste_mov32_r_i(void);
void funcao_teste_mov32_r_m(void);
void funcao_teste_mov32_r_mi(void);
void funcao_teste_mov32_r_mr(void);
void funcao_teste_mov32_r_r(void);
void funcao_teste_mov32_m_i(void);
void funcao_teste_mov32_m_r(void);
void funcao_teste_mov32_mi_i(void);
void funcao_teste_mov32_mi_r(void);
void funcao_teste_mov32_mr_i(void);
void funcao_teste_mov32_mr_r(void);

void funcao_teste_cmp32_r_r(void);
void funcao_teste_cmp32_r_i(void);
void funcao_teste_cmp32_r_m(void);
void funcao_teste_cmp32_r_mi(void);

// Teste de funções shift 32-bit
void funcao_teste_shl_reg_imm(void);
void funcao_teste_shr_reg_imm(void);
// Prototypes for register-based shift tests
void funcao_teste_lshfit32_reg(void);
void funcao_teste_rshfit32_reg(void);
void funcao_test_and32_r_r(void);
void funcao_teste_and32_r_i(void);
void funcao_teste_and32_r_m(void);
void funcao_teste_and32_r_mi(void);
void funcao_teste_and32_r_mr(void);
// OR 32-bit tests
void funcao_test_or32_r_r(void);
void funcao_teste_or32_r_i(void);
void funcao_teste_or32_r_m(void);
void funcao_teste_or32_r_mi(void);
void funcao_teste_or32_r_mr(void);

// XOR 32-bit tests
void funcao_teste_xor32_r_r(void);
void funcao_teste_xor32_r_i(void);
void funcao_teste_xor32_r_m(void);
void funcao_teste_xor32_r_mi(void);
void funcao_teste_xor32_r_mr(void);

// NOT 32-bit tests
void funcao_teste_not32_r(void);
void funcao_teste_not32_m(void);
void funcao_teste_not32_mi(void);
void funcao_teste_not32_mr(void);

// Arithmetic 32-bit tests (SUM, SUB, MUL, DIV)
// SUM 32-bit tests
void funcao_teste_sum32_r_r(void);
void funcao_teste_sum32_r_i(void);
void funcao_teste_sum32_r_m(void);
void funcao_teste_sum32_r_mi(void);
void funcao_teste_sum32_r_mr(void);

// SUB 32-bit tests
void funcao_teste_sub32_r_r(void);
void funcao_teste_sub32_r_i(void);
void funcao_teste_sub32_r_m(void);
void funcao_teste_sub32_r_mi(void);
void funcao_teste_sub32_r_mr(void);

// MUL 32-bit tests
void funcao_teste_mul32_r_r(void);
void funcao_teste_mul32_r_i(void);
void funcao_teste_mul32_r_m(void);
void funcao_teste_mul32_r_mi(void);
void funcao_teste_mul32_r_mr(void);

// DIV 32-bit tests
void funcao_teste_div32_r(void);
void funcao_teste_div32_m(void);
void funcao_teste_div32_mi(void);
void funcao_teste_div32_mr(void);

void funcao_teste_inc32_r(void);
void funcao_teste_inc32_m(void);
void funcao_teste_inc32_mi(void);
void funcao_teste_inc32_mr(void);

void funcao_teste_dec32_r(void);
void funcao_teste_dec32_m(void);
void funcao_teste_dec32_mi(void);
void funcao_teste_dec32_mr(void);

#endif // BFUNCTIONS32_H
#ifndef BFUNCTIONS32_H
#define BFUNCTIONS32_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define REG_EAX 0x0 // 32-bit register defs kept for compatibility
#define REG_ECX 0x1
#define REG_EDX 0x2
#define REG_EBX 0x3
#define REG_ESP 0x4
#define REG_EBP 0x5
#define REG_ESI 0x6
#define REG_EDI 0x7

#define XMM0 0x0
#define XMM1 0x1
#define XMM2 0x2
#define XMM3 0x3
#define XMM4 0x4
#define XMM5 0x5
#define XMM6 0x6
#define XMM7 0x7

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

// xmm
void mov_x_r(uint8_t xmmReg, uint8_t reg);
void mov_r_x(uint8_t xmmReg, uint8_t reg);
void cvtss2si(uint8_t reg, uint8_t xmmReg);
void cvtsi2ss(uint8_t xmmReg, uint8_t reg);
void ucomiss(uint8_t xmm1, uint8_t xmm2);
void addss(uint8_t xmm1, uint8_t xmm2);
void subss(uint8_t xmm1, uint8_t xmm2);
void mulss(uint8_t xmm1, uint8_t xmm2);
void divss(uint8_t xmm1, uint8_t xmm2);
void sqrtss(uint8_t xmm1, uint8_t xmm2);

void sete_r(uint8_t reg);
void setne_r(uint8_t reg);
void setl_r(uint8_t reg);
void setg_r(uint8_t reg);

void shl_reg_imm(uint8_t reg, uint8_t imm);
void shr_reg_imm(uint8_t reg, uint8_t imm);

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

#endif // BFUNCTIONS32_H
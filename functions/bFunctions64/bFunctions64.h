#ifndef BFUNCTIONS64_H
#define BFUNCTIONS64_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// General Purpose Registers (0-7)
#define REG_RAX 0x0
#define REG_RCX 0x1
#define REG_RDX 0x2
#define REG_RBX 0x3
#define REG_RSP 0x4
#define REG_RBP 0x5
#define REG_RSI 0x6
#define REG_RDI 0x7

// Extended Registers (8-15)
#define REG_R8 0x8  // o que importa sao os ultimos 3 bits ou seja continua a ser 000
#define REG_R9 0x9  // o que importa sao os ultimos 3 bits ou seja continua a ser 001
#define REG_R10 0xA // o que importa sao os ultimos 3 bits ou seja continua a ser 010
#define REG_R11 0xB // o que importa sao os ultimos 3 bits ou seja continua a ser 011
#define REG_R12 0xC // o que importa sao os ultimos 3 bits ou seja continua a ser 100
#define REG_R13 0xD // o que importa sao os ultimos 3 bits ou seja continua a ser 101
#define REG_R14 0xE // o que importa sao os ultimos 3 bits ou seja continua a ser 110
#define REG_R15 0xF // o que importa sao os ultimos 3 bits ou seja continua a ser 111
// esta assim so para saber que 8-15 sao difentes do a,c,d,b,s,b,s,d

// Special Registers (Use with caution, not for general mov instructions)
#define REG_RIP 0x10
#define REG_RFLAGS 0x11

// Move to/from registers
void mov64_r_i(uint8_t reg_code, uint64_t value);
void mov64_r_m(uint8_t reg, uint8_t mem_reg);
void mov64_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void mov64_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset);
void mov64_r_r(uint8_t reg1, uint8_t reg2);

// Move to memory
void mov64_m_i32(uint8_t reg1, uint32_t value);
void mov64_m_r(uint8_t reg1, uint8_t reg2);
void mov64_mi_i(uint8_t reg, int32_t offset, int32_t value);
void mov64_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2);
void mov64_mr_i(uint8_t reg, uint8_t reg2, int32_t value);
void mov64_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3);

// Function prototypes for system calls
void syscall_instruction();

// testes
void funcao_teste_mov64_r_i(void);
void funcao_teste_mov64_r_m(void);
void funcao_teste_mov64_r_mi(void);
void funcao_teste_mov64_r_mr(void);
void funcao_teste_mov64_r_r(void);
void funcao_teste_mov64_m_i32(void);
void funcao_teste_mov64_m_r(void);
void funcao_teste_mov64_mi_i(void);
void funcao_teste_mov64_mi_r(void);
void funcao_teste_mov64_mr_i(void);
void funcao_teste_mov64_mr_r(void);

void funcao_teste_cmp64_r_r(void);
void funcao_teste_cmp64_r_i(void);
void funcao_teste_cmp64_r_m(void);
void funcao_teste_cmp64_r_mi(void);
void funcao_teste_lshfit64(void);
void funcao_teste_rshfit64(void);
// Test register-based shift by CL
void funcao_teste_lshfit64_reg(void);
void funcao_teste_rshfit64_reg(void);
void funcao_teste_push64(void);
void funcao_teste_pop64(void);
void funcao_teste_push64_m(void);
void funcao_teste_push64_mi(void);
void funcao_teste_pop64_m(void);
void funcao_teste_pop64_mi(void);

void funcao_teste_and64_r_r(void);
void funcao_teste_and64_r_i(void);
void funcao_teste_and64_r_m(void);
void funcao_teste_and64_r_mi(void);
void funcao_teste_and64_r_mr(void);

// OR test prototypes
void funcao_teste_or64_r_r(void);
void funcao_teste_or64_r_i(void);
void funcao_teste_or64_r_m(void);
void funcao_teste_or64_r_mi(void);
void funcao_teste_or64_r_mr(void);

// NOT test prototypes
void funcao_teste_not64_r(void);
void funcao_teste_not64_m(void);
void funcao_teste_not64_mi(void);
void funcao_teste_not64_mr(void);

// Utility functions
void cmp64_r_r(uint8_t reg1, uint8_t reg2);
void cmp64_r_i(uint8_t reg1, uint32_t imm32);
void cmp64_r_m(uint8_t reg1, uint8_t reg2);
void cmp64_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void rshfit64(uint8_t reg, uint8_t imm);
void lshfit64(uint8_t reg, uint8_t imm);
void rshfit64_reg(uint8_t reg1);
void lshfit64_reg(uint8_t reg1);
void push64(uint8_t reg);
void pop64(uint8_t reg);
void push64_m(uint8_t reg);
void push64_mi(uint8_t reg, uint32_t offset);
void pop64_m(uint8_t reg);
void pop64_mi(uint8_t reg, uint32_t offset);
void and64_r_r(uint8_t reg1, uint8_t reg2);
void and64_r_i(uint8_t reg, uint32_t imm32);
void and64_r_m(uint8_t reg1, uint8_t reg2);
void and64_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void and64_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);


// OR utility function prototypes
void or64_r_r(uint8_t reg1, uint8_t reg2);
void or64_r_i(uint8_t reg, uint32_t imm32);
void or64_r_m(uint8_t reg1, uint8_t reg2);
void or64_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset);
void or64_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3);

// NOT utility function prototypes
void not64_r(uint8_t reg);
void not64_m(uint8_t reg);
void not64_mi(uint8_t reg, uint32_t offset);
void not64_mr(uint8_t reg_base, uint8_t reg_index);

void set_rex_prefix(char *opcode_bytes, uint8_t w, uint8_t r, uint8_t x, uint8_t b);

#endif

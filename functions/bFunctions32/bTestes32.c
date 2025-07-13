#include "bFunctions32.h"
#include "../functions.h"
#include <stdint.h>
#include <limits.h>

// 64-bit register definitions for memory operations
#define REG_RAX 0x0
#define REG_RCX 0x1
#define REG_RDX 0x2
#define REG_RBX 0x3
#define REG_RSP 0x4
#define REG_RBP 0x5
#define REG_RSI 0x6
#define REG_RDI 0x7

// Registos de 0 a 7 (32-bit registers for operations)
static const uint8_t all_regs[] = {
    REG_EAX, REG_ECX, REG_EDX, REG_EBX,
    REG_ESP, REG_EBP, REG_ESI, REG_EDI};

// 64-bit registers for memory addressing
static const uint8_t all_mem_regs[] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RBX,
    REG_RSP, REG_RBP, REG_RSI, REG_RDI};

// Valores imediatos para testar edge cases
static const uint32_t imm32_vals[] = {0U, UINT32_MAX, 1U, 0x80000000U};
// Offsets para testar edge cases
static const int32_t off_vals[] = {INT32_MIN, -1, 0, 1, INT32_MAX};

// check 0
//  Test de mov32_r_i: reg <- imediato 32-bit
void funcao_teste_mov32_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            mov32_r_i(r, imm32_vals[j]);
        }
        nop();
    }
}

// check 0
//  Test de mov32_r_m: reg <- mem[reg]
void funcao_teste_mov32_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            mov32_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// check 0
//  Test de mov32_r_mi: reg <- mem[reg_base + offset]
void funcao_teste_mov32_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                mov32_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  Test de mov32_r_mr: reg <- mem[reg_base + reg_offset]
void funcao_teste_mov32_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue;
                }
                mov32_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  Test de mov32_r_r: reg <- reg
void funcao_teste_mov32_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov32_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Move to memory tests:

// check 0
//  mov32_m_i: mem[reg1] <- valor imediato 32-bit
void funcao_teste_mov32_m_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            mov32_m_i(all_mem_regs[i], imm32_vals[j]);
        }
        nop();
    }
}

// check 0
//  mov32_m_r: mem[reg1] <- reg2
void funcao_teste_mov32_m_r(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov32_m_r(all_mem_regs[i], all_regs[j]);
        }
        nop();
    }
}

// check 0
//  mov32_mi_i: mem[reg + offset] <- valor imediato 32-bit
void funcao_teste_mov32_mi_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(imm32_vals) / sizeof(*imm32_vals); ++k)
            {
                mov32_mi_i(all_mem_regs[i], off_vals[j], imm32_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  mov32_mi_r: mem[reg + offset] <- reg2
void funcao_teste_mov32_mi_r(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                mov32_mi_r(all_mem_regs[i], off_vals[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  mov32_mr_i: mem[reg + reg2] <- valor imediato 32-bit
void funcao_teste_mov32_mr_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue;
            }
            for (unsigned k = 0; k < sizeof(imm32_vals) / sizeof(*imm32_vals); ++k)
            {
                mov32_mr_i(all_mem_regs[i], all_mem_regs[j], imm32_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  mov32_mr_r: mem[reg_base + reg2] <- reg3
void funcao_teste_mov32_mr_r(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue;
            }
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                mov32_mr_r(all_mem_regs[i], all_mem_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  Test de cmp32_r_r: compara registrador com registrador
void funcao_teste_cmp32_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            cmp32_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// check 0
//  Test de cmp32_r_i: compara registrador com imediato 32-bit
void funcao_teste_cmp32_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            cmp32_r_i(all_regs[i], imm32_vals[j]);
        }
        nop();
    }
}

// check 0
//  Test de cmp32_r_m: compara registrador com valor em memória [reg]
void funcao_teste_cmp32_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            cmp32_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// check 0
//  Test de cmp32_r_mi: compara registrador com valor em memória [reg+offset]
void funcao_teste_cmp32_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                cmp32_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}
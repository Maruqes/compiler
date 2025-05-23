#include "bFunctions64.h"
#include "../functions.h"
#include <stdint.h>
#include <limits.h>

// Registos de 0 a 15
static const uint8_t all_regs[] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RBX,
    REG_RSP, REG_RBP, REG_RSI, REG_RDI,
    REG_R8, REG_R9, REG_R10, REG_R11,
    REG_R12, REG_R13, REG_R14, REG_R15};
// Valores imediatos para testar edge cases
static const uint64_t imm64_vals[] = {0ULL, UINT64_MAX, 1ULL, 0x8000000000000000ULL};
static const uint32_t imm32_vals[] = {0U, UINT32_MAX, 1U, 0x80000000U};
// Offsets para testar edge cases
static const int32_t off_vals[] = {INT32_MIN, -1, 0, 1, INT32_MAX};

// Test de mov64_r_i: reg imediato 64-bit
void funcao_teste_mov64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm64_vals) / sizeof(*imm64_vals); ++j)
        {
            mov64_r_i(r, imm64_vals[j]);
        }
        nop();
    }
}

// Test de mov64_r_m: reg <- mem[reg]
void funcao_teste_mov64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov64_r_m(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test de mov64_r_mi: reg <- mem[reg_base + offset]
void funcao_teste_mov64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                mov64_r_mi(all_regs[i], all_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test de mov64_r_mr: reg <- mem[reg_base + reg_offset]
void funcao_teste_mov64_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                if (all_regs[k] == REG_RSP)
                {
                    continue;
                }
                mov64_r_mr(all_regs[i], all_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test de mov64_r_r: reg <- reg
void funcao_teste_mov64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov64_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Move to memory tests:

// mov64_m_i32: mem[reg1] <- valor imediato 32-bit
void funcao_teste_mov64_m_i32(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            mov64_m_i32(all_regs[i], imm32_vals[j]);
        }
        nop();
    }
}

// mov64_m_r: mem[reg1] <- reg2
void funcao_teste_mov64_m_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov64_m_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// mov64_mi_i: mem[reg + offset] <- valor imediato 32-bit
void funcao_teste_mov64_mi_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                mov64_mi_i(all_regs[i], off_vals[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// mov64_mi_r: mem[reg + offset] <- reg2
void funcao_teste_mov64_mi_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                mov64_mi_r(all_regs[i], off_vals[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// mov64_mr_i: mem[reg + reg2] <- valor imediato 32-bit
void funcao_teste_mov64_mr_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if(all_regs[j] == REG_RSP)
            {
                continue;
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                mov64_mr_i(all_regs[i], all_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// mov64_mr_r: mem[reg_base + reg2] <- reg3
void funcao_teste_mov64_mr_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if(all_regs[j] == REG_RSP)
            {
                continue;
            }
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                mov64_mr_r(all_regs[i], all_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}
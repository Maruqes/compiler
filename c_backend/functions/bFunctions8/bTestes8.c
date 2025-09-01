#include "bFunctions8.h"
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
#define REG_R8 0x8
#define REG_R9 0x9
#define REG_R10 0xA
#define REG_R11 0xB
#define REG_R12 0xC
#define REG_R13 0xD
#define REG_R14 0xE
#define REG_R15 0xF

// Registos de 0 a 7 (8-bit registers for operations)
static const uint8_t all_regs[] = {
    REG_AL, REG_CL, REG_DL, REG_BL, REG_SPL, REG_BPL,
    REG_SIL, REG_DIL, REG_R8B, REG_R9B, REG_R10B, REG_R11B, REG_R12B};

// 64-bit registers for memory addressing
static const uint8_t all_mem_regs[] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RBX,
    REG_RSP, REG_RBP, REG_RSI, REG_RDI,
    REG_R8, REG_R9, REG_R10, REG_R11, REG_R12};

// Valores imediatos para testar edge cases
static const uint8_t imm8_vals[] = {0U, UINT8_MAX, 1U, 0x80U};
// Offsets para testar edge cases
static const int32_t off_vals[] = {INT32_MIN, -1, 0, 1, INT32_MAX};

// check 0
//  Test de mov8_r_i: reg <- imediato 8-bit
void funcao_teste_mov8_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            mov8_r_i(r, imm8_vals[j]);
        }
        nop();
    }
}

// check 0
//  Test de mov8_r_m: reg <- mem[reg]
void funcao_teste_mov8_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            mov8_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// check 0
//  Test de mov8_r_mi: reg <- mem[reg_base + offset]
void funcao_teste_mov8_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                mov8_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  Test de mov8_r_mr: reg <- mem[reg_base + reg_offset]
void funcao_teste_mov8_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP || all_mem_regs[k] == REG_R12B)
                {
                    continue;
                }
                mov8_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  Test de mov8_r_r: reg <- reg
void funcao_teste_mov8_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov8_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Move to memory tests:

// check0
//  mov8_m_i: mem[reg1] <- valor imediato 8-bit
void funcao_teste_mov8_m_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            mov8_m_i(all_mem_regs[i], imm8_vals[j]);
        }
        nop();
    }
}

// check 0
//  mov8_m_r: mem[reg1] <- reg2
void funcao_teste_mov8_m_r(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov8_m_r(all_mem_regs[i], all_regs[j]);
        }
        nop();
    }
}

// check 0
//  mov8_mi_i: mem[reg + offset] <- valor imediato 8-bit
void funcao_teste_mov8_mi_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(imm8_vals) / sizeof(*imm8_vals); ++k)
            {
                mov8_mi_i(all_mem_regs[i], off_vals[j], imm8_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//  mov8_mi_r: mem[reg + offset] <- reg2
void funcao_teste_mov8_mi_r(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                mov8_mi_r(all_mem_regs[i], off_vals[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// mov8_mr_i: mem[reg + reg2] <- valor imediato 8-bit
void funcao_teste_mov8_mr_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue;
            }
            for (unsigned k = 0; k < sizeof(imm8_vals) / sizeof(*imm8_vals); ++k)
            {
                mov8_mr_i(all_mem_regs[i], all_mem_regs[j], imm8_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// mov8_mr_r: mem[reg_base + reg2] <- reg3
void funcao_teste_mov8_mr_r(void)
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
                mov8_mr_r(all_mem_regs[i], all_mem_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Compare functions tests:

// Test de cmp8_r_r: cmp reg1, reg2
void funcao_teste_cmp8_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            cmp8_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test de cmp8_r_i: cmp reg, imm8
void funcao_teste_cmp8_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            cmp8_r_i(r, imm8_vals[j]);
        }
        nop();
    }
}

// Test de cmp8_r_m: cmp reg, [mem_reg]
void funcao_teste_cmp8_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            cmp8_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test de cmp8_r_mi: cmp reg, [mem_reg + offset]
void funcao_teste_cmp8_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                cmp8_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test de rshfit8: shr reg, imm8
void funcao_teste_rshfit8(void)
{
    static const uint8_t shift_counts[] = {0U, 1U, 3U, 7U}; // Valid shift counts for 8-bit (0-7)
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(shift_counts) / sizeof(*shift_counts); ++j)
        {
            rshfit8(all_regs[i], shift_counts[j]);
        }
        nop();
    }
}

// Test de lshfit8_reg: shl reg, CL
void funcao_teste_lshfit8_reg(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        lshfit8_reg(all_regs[i]);
        nop();
    }
}

// Test de rshfit8_reg: shr reg, CL
void funcao_teste_rshfit8_reg(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        rshfit8_reg(all_regs[i]);
        nop();
    }
}

// Shift functions tests:

// Test de lshfit8: shl reg, imm8
void funcao_teste_lshfit8(void)
{
    // Test values for shift amounts (0-7 for 8-bit shifts)
    static const uint8_t shift_vals[] = {0, 1, 2, 3, 4, 5, 6, 7};

    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(shift_vals) / sizeof(*shift_vals); ++j)
        {
            lshfit8(all_regs[i], shift_vals[j]);
        }
        nop();
    }
}

// AND functions tests:

// Test de and8_r_r: and reg1, reg2
void funcao_teste_and8_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            and8_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test de and8_r_i: and reg, imm8
void funcao_teste_and8_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            and8_r_i(r, imm8_vals[j]);
        }
        nop();
    }
}

// Test de and8_r_m: and reg, [mem_reg]
void funcao_teste_and8_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            and8_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test de and8_r_mi: and reg, [mem_reg + offset]
void funcao_teste_and8_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                and8_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test de and8_r_mr: and reg, [reg_base + reg_index]
void funcao_teste_and8_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as base register
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }

                and8_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// OR functions tests:

// Test de or8_r_r: or reg1, reg2
void funcao_teste_or8_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            or8_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test de or8_r_i: or reg, imm8
void funcao_teste_or8_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            or8_r_i(r, imm8_vals[j]);
        }
        nop();
    }
}

// Test de or8_r_m: or reg, [mem_reg]
void funcao_teste_or8_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            or8_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test de or8_r_mi: or reg, [mem_reg + offset]
void funcao_teste_or8_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                or8_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test de or8_r_mr: or reg, [base_reg + index_reg]
void funcao_teste_or8_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as index register
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }

                or8_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// NOT functions tests:

// Test de not8_r: not reg
void funcao_teste_not8_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        not8_r(all_regs[i]);
        nop();
    }
}

// Test de not8_m: not [mem_reg]
void funcao_teste_not8_m(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        not8_m(all_mem_regs[i]);
        nop();
    }
}

// Test de not8_mi: not [mem_reg + offset]
void funcao_teste_not8_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            not8_mi(all_mem_regs[i], off_vals[j]);
        }
        nop();
    }
}

// Test de not8_mr: not [base_reg + index_reg]
void funcao_teste_not8_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as base register
        }
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as index register
            }
            not8_mr(all_mem_regs[i], all_mem_regs[j]);
            nop();
        }
        nop();
        nop();
    }
}

// XOR functions tests:

// Test de xor8_r_r: xor reg1, reg2
void funcao_teste_xor8_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            xor8_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test de xor8_r_i: xor reg, imm8
void funcao_teste_xor8_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            xor8_r_i(r, imm8_vals[j]);
        }
        nop();
    }
}

// Test de xor8_r_m: xor reg, [mem_reg]
void funcao_teste_xor8_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            xor8_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test de xor8_r_mi: xor reg, [mem_reg + offset]
void funcao_teste_xor8_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                xor8_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test de xor8_r_mr: xor reg, [base_reg + index_reg]
void funcao_teste_xor8_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                if (all_mem_regs[j] == REG_RSP)
                {
                    continue; // Skip RSP as base register
                }
                xor8_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// INC tests
void funcao_teste_inc8_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        inc8_r(all_regs[i]);
        nop();
    }
}

void funcao_teste_inc8_m(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        inc8_m(all_mem_regs[i]);
        nop();
    }
}

void funcao_teste_inc8_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            inc8_mi(all_mem_regs[i], off_vals[j]);
        }
        nop();
    }
}

void funcao_teste_inc8_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as base register
        }
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as index register
            }
            inc8_mr(all_mem_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// DEC tests
void funcao_teste_dec8_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        dec8_r(all_regs[i]);
        nop();
    }
}

void funcao_teste_dec8_m(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        dec8_m(all_mem_regs[i]);
        nop();
    }
}

void funcao_teste_dec8_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            dec8_mi(all_mem_regs[i], off_vals[j]);
        }
        nop();
    }
}

void funcao_teste_dec8_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as base register
        }
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as index register
            }
            dec8_mr(all_mem_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// ============================================================================
// ARITHMETIC TESTS (SUM, SUB, MUL, DIV)
// ============================================================================

// SUM 8-bit tests

// Test SUM r, r
void funcao_teste_sum8_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            sum8_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test SUM r, imm8
void funcao_teste_sum8_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            sum8_r_i(r, imm8_vals[j]);
        }
        nop();
    }
}

// Test SUM r, [mem]
void funcao_teste_sum8_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            sum8_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test SUM r, [mem + offset]
void funcao_teste_sum8_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                sum8_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test SUM r, [mem + reg]
void funcao_teste_sum8_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as base register
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                sum8_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// SUB 8-bit tests

// Test SUB r, r
void funcao_teste_sub8_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            sub8_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test SUB r, imm8
void funcao_teste_sub8_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm8_vals) / sizeof(*imm8_vals); ++j)
        {
            sub8_r_i(r, imm8_vals[j]);
        }
        nop();
    }
}

// Test SUB r, [mem]
void funcao_teste_sub8_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            sub8_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test SUB r, [mem + offset]
void funcao_teste_sub8_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as memory register
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                sub8_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test SUB r, [mem + reg]
void funcao_teste_sub8_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as base register
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                sub8_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// MUL 8-bit tests

// Test MUL r (multiplies AL by r)
void funcao_teste_mul8_r(void)
{
    for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
    {
        mul8_r(all_regs[j]);
    }
}

// Test MUL [mem] (multiplies AL by [mem])
void funcao_teste_mul8_m(void)
{
    for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
    {
        if (all_mem_regs[j] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        mul8_m(all_mem_regs[j]);
    }
}

// Test MUL [mem + offset] (multiplies AL by [mem + offset])
void funcao_teste_mul8_mi(void)
{
    for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
    {
        if (all_mem_regs[j] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
        {
            mul8_mi(all_mem_regs[j], off_vals[k]);
        }
        nop();
    }
    nop();
}

// Test MUL [mem + reg] (multiplies AL by [mem + reg])
void funcao_teste_mul8_mr(void)
{
    for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
    {
        if (all_mem_regs[j] == REG_RSP)
        {
            continue; // Skip RSP as base register
        }
        for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
        {
            if (all_mem_regs[k] == REG_RSP)
            {
                continue; // Skip RSP as index register
            }
            mul8_mr(all_mem_regs[j], all_mem_regs[k]);
        }
        nop();
    }
    nop();
}

// DIV 8-bit tests

// Test DIV r (divides AX by r)
void funcao_teste_div8_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        div8_r(all_regs[i]);
        nop();
    }
}

// Test DIV [mem] (divides AX by [mem])
void funcao_teste_div8_m(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        div8_m(all_mem_regs[i]);
        nop();
    }
}

// Test DIV [mem + offset] (divides AX by [mem + offset])
void funcao_teste_div8_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as memory register
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            div8_mi(all_mem_regs[i], off_vals[j]);
        }
        nop();
        nop();
    }
}

// Test DIV [mem + reg] (divides AX by [mem + reg])
void funcao_teste_div8_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as base register
        }
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as index register
            }
            div8_mr(all_mem_regs[i], all_mem_regs[j]);
        }
        nop();
        nop();
    }
}

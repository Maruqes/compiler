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
            if (all_regs[j] == REG_RSP)
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
            if (all_regs[j] == REG_RSP)
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

/*

   TEST UTILS

   TEST UTILS

   TEST UTILS

   TEST UTILS

*/

// test cmp64_r_r: cmp reg1, reg2
void funcao_teste_cmp64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            cmp64_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// test cmp64_r_i: cmp reg, imm64
void funcao_teste_cmp64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm64_vals) / sizeof(*imm64_vals); ++j)
        {
            cmp64_r_i(all_regs[i], imm64_vals[j]);
        }
        nop();
    }
}

// test cmp64_r_m: cmp reg, mem[reg]
void funcao_teste_cmp64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            cmp64_r_m(all_regs[i], all_regs[j]);
        }
        nop();
    }
}
void funcao_teste_cmp64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                cmp64_r_mi(all_regs[i], all_regs[j], off_vals[k]);
            }
        }
        nop();
    }
}

void funcao_teste_lshfit64(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < 64; ++j) // Shift from 0 to 63
        {
            lshfit64(all_regs[i], j);
        }
        nop();
    }
}

void funcao_teste_rshfit64(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < 64; ++j) // Shift from 0 to 63
        {
            rshfit64(all_regs[i], j);
        }
        nop();
    }
}

// test shift left by CL register (lshfit64_reg)
void funcao_teste_lshfit64_reg(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        lshfit64_reg(all_regs[i]);
    }
}

// test shift right by CL register (rshfit64_reg)
void funcao_teste_rshfit64_reg(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        rshfit64_reg(all_regs[i]);
    }
}

void funcao_teste_push64(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        push64(all_regs[i]);
        nop();
    }
}

void funcao_teste_pop64(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        pop64(all_regs[i]);
        nop();
    }
}

void funcao_teste_push64_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        if (all_regs[i] == REG_RSP)
        {
            continue;
        }
        push64_m(all_regs[i]);
        nop();
    }
}

void funcao_teste_push64_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        if (all_regs[i] == REG_RSP)
        {
            continue;
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            push64_mi(all_regs[i], off_vals[j]);
        }
    }
}

void funcao_teste_pop64_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        if (all_regs[i] == REG_RSP)
        {
            continue;
        }
        pop64_m(all_regs[i]);
        nop();
    }
}

void funcao_teste_pop64_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        if (all_regs[i] == REG_RSP)
        {
            continue;
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            pop64_mi(all_regs[i], off_vals[j]);
        }
    }
}

/*

   TEST AND64 FUNCTIONS

   TEST AND64 FUNCTIONS

   TEST AND64 FUNCTIONS

   TEST AND64 FUNCTIONS

*/

// test and64_r_r: and reg1, reg2
void funcao_teste_and64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            and64_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// test and64_r_i: and reg, imm32
void funcao_teste_and64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            and64_r_i(all_regs[i], imm32_vals[j]);
        }
        nop();
    }
}

// test and64_r_m: and reg, mem[reg]
void funcao_teste_and64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            and64_r_m(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// test and64_r_mi: and reg, mem[reg + offset]
void funcao_teste_and64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                and64_r_mi(all_regs[i], all_regs[j], off_vals[k]);
            }
        }
        nop();
    }
}

// test and64_r_mr: and reg, mem[reg_base + reg_offset]
void funcao_teste_and64_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                if (all_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                and64_r_mr(all_regs[i], all_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
    }
}

// OR64 TESTS

// test or64_r_r: or reg1, reg2
void funcao_teste_or64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            or64_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// test or64_r_i: or reg, imm32
void funcao_teste_or64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            or64_r_i(all_regs[i], imm32_vals[j]);
        }
        nop();
    }
}

// test or64_r_m: or reg, mem[reg]
void funcao_teste_or64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            or64_r_m(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// test or64_r_mi: or reg, mem[reg + offset]
void funcao_teste_or64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                or64_r_mi(all_regs[i], all_regs[j], off_vals[k]);
            }
        }
        nop();
    }
}

// test or64_r_mr: or reg, mem[reg_base + reg_offset]
void funcao_teste_or64_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                if (all_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                or64_r_mr(all_regs[i], all_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
    }
}

// XOR64 TESTS

// test xor64_r_r: xor reg, reg
void funcao_teste_xor64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            xor64_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// test xor64_r_i: xor reg, imm32
void funcao_teste_xor64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            xor64_r_i(all_regs[i], imm32_vals[j]);
        }
        nop();
    }
}

// test xor64_r_m: xor reg, mem[reg]
void funcao_teste_xor64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            xor64_r_m(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// test xor64_r_mi: xor reg, mem[reg + offset]
void funcao_teste_xor64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                xor64_r_mi(all_regs[i], all_regs[j], off_vals[k]);
            }
        }
        nop();
    }
}

// test xor64_r_mr: xor reg, mem[reg_base + reg_offset]
void funcao_teste_xor64_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                if (all_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                xor64_r_mr(all_regs[i], all_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
    }
}

// NOT64 TESTS

// test not64_r: not reg
void funcao_teste_not64_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        not64_r(all_regs[i]);
    }
    nop();
}

// test not64_m: not [reg]
void funcao_teste_not64_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        if (all_regs[i] == REG_RSP)
        {
            continue;
        }
        not64_m(all_regs[i]);
    }
    nop();
}

// test not64_mi: not [reg + offset]
void funcao_teste_not64_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        if (all_regs[i] == REG_RSP)
        {
            continue;
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            not64_mi(all_regs[i], off_vals[j]);
        }
        nop();
    }
}

// test not64_mr: not [reg_base + reg_index]
void funcao_teste_not64_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[i] == REG_RSP || all_regs[j] == REG_RSP)
            {
                continue;
            }
            not64_mr(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// TEST SUM64 FUNCTIONS

// test sum64_r_r: add reg1, reg2
void funcao_teste_sum64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            sum64_r_r(r1, r2);
        }
        nop();
    }
}

// test sum64_r_i: add reg, imm64
void funcao_teste_sum64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            // Using 32-bit values since our implementation limits to 32-bit immediate
            sum64_r_i(r, (uint64_t)imm32_vals[j]);
        }
        nop();
    }
}

// test sum64_r_m: add reg, [reg]
void funcao_teste_sum64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            sum64_r_m(r1, r2);
        }
        nop();
    }
}

// test sum64_r_mi: add reg, [reg + offset]
void funcao_teste_sum64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                // Convert signed to unsigned for the function call
                uint64_t offset = (uint64_t)(uint32_t)off_vals[k];
                sum64_r_mi(r1, r2, offset);
            }
        }
        nop();
    }
}

// test sum64_r_mr: add reg, [reg_base + reg_index]
void funcao_teste_sum64_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                if (all_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                uint8_t r3 = all_regs[k];
                sum64_r_mr(r1, r2, r3);
            }
        }
        nop();
    }
}

// TEST SUB64 FUNCTIONS

// test sub64_r_r: sub reg1, reg2
void funcao_teste_sub64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            sub64_r_r(r1, r2);
        }
        nop();
    }
}

// test sub64_r_i: sub reg, imm64
void funcao_teste_sub64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm32_vals) / sizeof(*imm32_vals); ++j)
        {
            // Using 32-bit values since our implementation limits to 32-bit immediate
            sub64_r_i(r, (uint64_t)imm32_vals[j]);
        }
        nop();
    }
}

// test sub64_r_m: sub reg, [reg]
void funcao_teste_sub64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            sub64_r_m(r1, r2);
        }
        nop();
    }
}

// test sub64_r_mi: sub reg, [reg + offset]
void funcao_teste_sub64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                // Convert signed to unsigned for the function call
                uint64_t offset = (uint64_t)(uint32_t)off_vals[k];
                sub64_r_mi(r1, r2, offset);
            }
        }
        nop();
    }
}

// test sub64_r_mr: sub reg, [reg_base + reg_index]
void funcao_teste_sub64_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                if (all_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                uint8_t r3 = all_regs[k];
                sub64_r_mr(r1, r2, r3);
            }
        }
        nop();
    }
}

// TEST MUL64 FUNCTIONS

// test mul64_r_r: imul reg1, reg2
void funcao_teste_mul64_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            mul64_r_r(r1, r2);
        }
        nop();
    }
}

// test mul64_r_i: imul reg, imm64
void funcao_teste_mul64_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm64_vals) / sizeof(*imm64_vals); ++j)
        {
            uint64_t imm = imm64_vals[j];
            if (imm <= UINT32_MAX) // Only test values that fit in 32 bits
            {
                mul64_r_i(r, imm);
            }
        }
        nop();
    }
}

// test mul64_r_m: imul reg, [reg]
void funcao_teste_mul64_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            mul64_r_m(r1, r2);
        }
        nop();
    }
}

// test mul64_r_mi: imul reg, [reg + offset]
void funcao_teste_mul64_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                int32_t offset = off_vals[k];
                if (offset >= 0) // Only test positive offsets for safety
                {
                    mul64_r_mi(r1, r2, (uint64_t)offset);
                }
            }
        }
        nop();
    }
}

// test mul64_r_mr: imul reg, [reg_base + reg_index]
void funcao_teste_mul64_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            uint8_t r2 = all_regs[j];
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                if (all_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as index register
                }
                uint8_t r3 = all_regs[k];
                mul64_r_mr(r1, r2, r3);
            }
        }
        nop();
    }
}

// TEST SINGLE-OPERAND DIV64 FUNCTIONS

// test div64_r: idiv reg (single operand IDIV)
void funcao_teste_div64_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        div64_r(r);
        nop();
    }
}

// test div64_m: idiv [reg] (single operand IDIV)
void funcao_teste_div64_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        div64_m(r);
        nop();
    }
}

// test div64_mi: idiv [reg + offset] (single operand IDIV)
void funcao_teste_div64_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            int32_t offset = off_vals[j];
            if (offset >= 0) // Only test positive offsets for safety
            {
                div64_mi(r, (uint64_t)offset);
            }
        }
        nop();
    }
}

// test div64_mr: idiv [reg_base + reg_index] (single operand IDIV)
void funcao_teste_div64_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r1 = all_regs[i];
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            if (all_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as index register
            }
            uint8_t r2 = all_regs[j];
            div64_mr(r1, r2);
        }
        nop();
    }
}

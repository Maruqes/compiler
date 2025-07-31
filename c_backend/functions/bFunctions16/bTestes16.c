#include "bFunctions16.h"
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

// Registos de 0 a 7 (16-bit registers for operations)
static const uint8_t all_regs[] = {
    REG_AX, REG_CX, REG_DX, REG_BX,
    REG_SP, REG_BP, REG_SI, REG_DI};

// 64-bit registers for memory addressing
static const uint8_t all_mem_regs[] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RBX,
    REG_RSP, REG_RBP, REG_RSI, REG_RDI};

// Valores imediatos para testar edge cases
static const uint16_t imm16_vals[] = {0U, UINT16_MAX, 1U, 0x8000U};
// Offsets para testar edge cases
static const int32_t off_vals[] = {INT32_MIN, -1, 0, 1, INT32_MAX};

// check 0
//  Test de mov16_r_i: reg <- imediato 16-bit
void funcao_teste_mov16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        uint8_t r = all_regs[i];
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            mov16_r_i(r, imm16_vals[j]);
        }
        nop();
    }
}

// check 0
//  Test de mov16_r_m: reg <- mem[reg]
void funcao_teste_mov16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            mov16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// check 0
//   Test de mov16_r_mi: reg <- mem[reg_base + offset]
void funcao_teste_mov16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                mov16_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//   Test de mov16_r_mr: reg <- mem[reg_base + reg_offset]
void funcao_teste_mov16_r_mr(void)
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
                mov16_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// check 0
//   Test de mov16_r_r: reg <- reg
void funcao_teste_mov16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Move to memory tests:

// check 0
//   mov16_m_i: mem[reg1] <- valor imediato 16-bit
void funcao_teste_mov16_m_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            mov16_m_i(all_mem_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// check 0
//   mov16_m_r: mem[reg1] <- reg2
void funcao_teste_mov16_m_r(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mov16_m_r(all_mem_regs[i], all_regs[j]);
        }
        nop();
    }
}

// check 0
//   mov16_mi_i: mem[reg + offset] <- valor imediato 16-bit
void funcao_teste_mov16_mi_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(imm16_vals) / sizeof(*imm16_vals); ++k)
            {
                mov16_mi_i(all_mem_regs[i], off_vals[j], imm16_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

//  mov16_mi_r: mem[reg + offset] <- reg2
void funcao_teste_mov16_mi_r(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            for (unsigned k = 0; k < sizeof(all_regs) / sizeof(*all_regs); ++k)
            {
                mov16_mi_r(all_mem_regs[i], off_vals[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

//  mov16_mr_i: mem[reg + reg2] <- valor imediato 16-bit
void funcao_teste_mov16_mr_i(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue;
            }
            for (unsigned k = 0; k < sizeof(imm16_vals) / sizeof(*imm16_vals); ++k)
            {
                mov16_mr_i(all_mem_regs[i], all_mem_regs[j], imm16_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

//  mov16_mr_r: mem[reg_base + reg2] <- reg3
void funcao_teste_mov16_mr_r(void)
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
                mov16_mr_r(all_mem_regs[i], all_mem_regs[j], all_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Compare function tests:

// Test cmp16_r_r: compare register to register
void funcao_teste_cmp16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            cmp16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test cmp16_r_i: compare register to immediate 16-bit value
void funcao_teste_cmp16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            cmp16_r_i(all_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// Test cmp16_r_m: compare register to memory [reg]
void funcao_teste_cmp16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            cmp16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test cmp16_r_mi: compare register to memory [reg + offset]
void funcao_teste_cmp16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                cmp16_r_mi(all_regs[i], all_mem_regs[j], (uint16_t)off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Shift function tests:

// Shift amounts for testing (both edge cases and normal values)
static const uint8_t shift_amounts[] = {0, 1, 7, 8, 15, 16, 31, 32};

// Test lshfit16: left shift register by immediate value
void funcao_teste_lshfit16(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(shift_amounts) / sizeof(*shift_amounts); ++j)
        {
            lshfit16(all_regs[i], shift_amounts[j]);
        }
        nop();
    }
}

// Test rshfit16: right shift register by immediate value
void funcao_teste_rshfit16(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(shift_amounts) / sizeof(*shift_amounts); ++j)
        {
            rshfit16(all_regs[i], shift_amounts[j]);
        }
        nop();
    }
}

// Test lshfit16_reg: left shift register by CL register
void funcao_teste_lshfit16_reg(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        lshfit16_reg(all_regs[i]);
        nop();
    }
}

// Test rshfit16_reg: right shift register by CL register
void funcao_teste_rshfit16_reg(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        rshfit16_reg(all_regs[i]);
        nop();
    }
}

// AND function tests:

// Test and16_r_r: AND register with register
void funcao_teste_and16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            and16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test and16_r_i: AND register with immediate 16-bit value
void funcao_teste_and16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            and16_r_i(all_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// Test and16_r_m: AND register with memory [reg]
void funcao_teste_and16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            and16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test and16_r_mi: AND register with memory [reg + offset]
void funcao_teste_and16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                and16_r_mi(all_regs[i], all_mem_regs[j], (uint16_t)off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test and16_r_mr: AND register with memory [reg_base + reg_index]
void funcao_teste_and16_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as it cannot be used as an index register
                }
                and16_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// OR function tests:

// Test or16_r_r: OR register with register
void funcao_teste_or16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            or16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test or16_r_i: OR register with immediate 16-bit value
void funcao_teste_or16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            or16_r_i(all_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// Test or16_r_m: OR register with memory [reg]
void funcao_teste_or16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            or16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test or16_r_mi: OR register with memory [reg + offset]
void funcao_teste_or16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                or16_r_mi(all_regs[i], all_mem_regs[j], (uint32_t)off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test or16_r_mr: OR register with memory [reg_base + reg_index]
void funcao_teste_or16_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as it cannot be used as an index register
                }
                or16_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// NOT function tests:

// Test not16_r: NOT register
void funcao_teste_not16_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        not16_r(all_regs[i]);
        nop();
    }
}

// Test not16_m: NOT memory [reg]
void funcao_teste_not16_m(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as it cannot be used as a memory operand
        }
        not16_m(all_mem_regs[i]);
        nop();
    }
}

// Test not16_mi: NOT memory [reg + offset]
void funcao_teste_not16_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as it cannot be used as a memory operand
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            not16_mi(all_mem_regs[i], (uint32_t)off_vals[j]);
        }
        nop();
        nop();
    }
}

// Test not16_mr: NOT memory [reg_base + reg_index]
void funcao_teste_not16_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as it cannot be used as a memory operand
        }
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as an index register
            }
            not16_mr(all_mem_regs[i], all_mem_regs[j]);
        }
        nop();
        nop();
    }
}

// XOR function tests:

// Test xor16_r_r: XOR register with register
void funcao_teste_xor16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            xor16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test xor16_r_i: XOR register with immediate 16-bit value
void funcao_teste_xor16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            xor16_r_i(all_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// Test xor16_r_m: XOR register with memory [reg]
void funcao_teste_xor16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            xor16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test xor16_r_mi: XOR register with memory [reg + offset]
void funcao_teste_xor16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                xor16_r_mi(all_regs[i], all_mem_regs[j], off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test xor16_r_mr: XOR register with memory [reg_base + reg_index]
void funcao_teste_xor16_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as it cannot be used as an index register
                }
                xor16_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// ============================================================================
// ARITHMETIC TESTS (SUM, SUB, MUL, DIV)
// ============================================================================

// SUM 16-bit tests

// Test SUM r, r
void funcao_teste_sum16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            sum16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test SUM r, imm16
void funcao_teste_sum16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            sum16_r_i(all_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// Test SUM r, [mem]
void funcao_teste_sum16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            sum16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test SUM r, [mem + offset]
void funcao_teste_sum16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                sum16_r_mi(all_regs[i], all_mem_regs[j], (uint32_t)off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test SUM r, [mem + reg]
void funcao_teste_sum16_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as it cannot be used as an index register
                }
                sum16_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// SUB 16-bit tests

// Test SUB r, r
void funcao_teste_sub16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            sub16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test SUB r, imm16
void funcao_teste_sub16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            sub16_r_i(all_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// Test SUB r, [mem]
void funcao_teste_sub16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            sub16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test SUB r, [mem + offset]
void funcao_teste_sub16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                sub16_r_mi(all_regs[i], all_mem_regs[j], (uint32_t)off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test SUB r, [mem + reg]
void funcao_teste_sub16_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as it cannot be used as an index register
                }
                sub16_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// MUL 16-bit tests

// Test MUL r, r
void funcao_teste_mul16_r_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_regs) / sizeof(*all_regs); ++j)
        {
            mul16_r_r(all_regs[i], all_regs[j]);
        }
        nop();
    }
}

// Test MUL r, imm16
void funcao_teste_mul16_r_i(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(imm16_vals) / sizeof(*imm16_vals); ++j)
        {
            mul16_r_i(all_regs[i], imm16_vals[j]);
        }
        nop();
    }
}

// Test MUL r, [mem]
void funcao_teste_mul16_r_m(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            mul16_r_m(all_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

// Test MUL r, [mem + offset]
void funcao_teste_mul16_r_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(off_vals) / sizeof(*off_vals); ++k)
            {
                mul16_r_mi(all_regs[i], all_mem_regs[j], (uint32_t)off_vals[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// Test MUL r, [mem + reg]
void funcao_teste_mul16_r_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as a memory operand
            }
            for (unsigned k = 0; k < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++k)
            {
                if (all_mem_regs[k] == REG_RSP)
                {
                    continue; // Skip RSP as it cannot be used as an index register
                }
                mul16_r_mr(all_regs[i], all_mem_regs[j], all_mem_regs[k]);
            }
            nop();
        }
        nop();
        nop();
    }
}

// DIV 16-bit tests

// Test DIV r (divides DX:AX by r)
void funcao_teste_div16_r(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        // Note: Division requires proper setup of DX:AX before calling
        // In a real scenario, you'd need to set up the dividend properly
        div16_r(all_regs[i]);
        nop();
    }
}

// Test DIV [mem] (divides DX:AX by [mem])
void funcao_teste_div16_m(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as it cannot be used as a memory operand
        }
        div16_m(all_mem_regs[i]);
        nop();
    }
}

// Test DIV [mem + offset] (divides DX:AX by [mem + offset])
void funcao_teste_div16_mi(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as it cannot be used as a memory operand
        }
        for (unsigned j = 0; j < sizeof(off_vals) / sizeof(*off_vals); ++j)
        {
            // Only test positive offsets for safety
            if (off_vals[j] >= 0)
            {
                div16_mi(all_mem_regs[i], (uint32_t)off_vals[j]);
            }
        }
        nop();
    }
}

// Test DIV [mem + reg] (divides DX:AX by [mem + reg])
void funcao_teste_div16_mr(void)
{
    for (unsigned i = 0; i < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++i)
    {
        if (all_mem_regs[i] == REG_RSP)
        {
            continue; // Skip RSP as it cannot be used as a memory operand
        }
        for (unsigned j = 0; j < sizeof(all_mem_regs) / sizeof(*all_mem_regs); ++j)
        {
            if (all_mem_regs[j] == REG_RSP)
            {
                continue; // Skip RSP as it cannot be used as an index register
            }
            div16_mr(all_mem_regs[i], all_mem_regs[j]);
        }
        nop();
    }
}

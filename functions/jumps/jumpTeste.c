#include "jumps.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
#include <stdint.h>
#include <limits.h>

static const uint8_t all_regs[] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RBX,
    REG_RSP, REG_RBP, REG_RSI, REG_RDI,
    REG_R8, REG_R9, REG_R10, REG_R11,
    REG_R12, REG_R13, REG_R14, REG_R15};

static const uint8_t short_offsets[] = {0, 1, 127, 128, 255};

static const uint32_t near_offsets[] = {0U, 1U, 0x7FFFFFFFU, 0x80000000U, UINT32_MAX};

void funcao_teste_jump_short(void)
{
    for (unsigned i = 0; i < sizeof(short_offsets) / sizeof(*short_offsets); ++i)
    {
        jump_short(short_offsets[i]);
        nop(); 
    }
}

void funcao_teste_jump_near(void)
{
    for (unsigned i = 0; i < sizeof(near_offsets) / sizeof(*near_offsets); ++i)
    {
        jump_near(near_offsets[i]);
        nop(); 
    }
}

void funcao_teste_jump_reg(void)
{
    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        if (all_regs[i] == REG_RSP)
        {
            continue;
        }
        jump_reg(all_regs[i]);
        nop(); 
    }
}
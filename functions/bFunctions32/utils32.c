#include "bFunctions32.h"
#include "../functions.h"

void cmp32_r_r(uint8_t reg1, uint8_t reg2)
{
}

void cmp32_r_i(uint8_t reg1, uint32_t imm32)
{
}

void cmp32_r_m(uint8_t reg1, uint8_t reg2)
{
}

void cmp32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
}

void lshfit32(uint8_t reg, uint8_t imm)
{
}

void rshfit32(uint8_t reg, uint8_t imm)
{
}

void push32(uint8_t reg)
{
}

void push32_m(uint8_t reg)
{
}

void push32_mi(uint8_t reg, uint32_t offset)
{
}

void pop32(uint8_t reg)
{
}

void pop32_m(uint8_t reg)
{
}

void pop32_mi(uint8_t reg, uint32_t offset)
{
}

// Logical operations: AND, OR, XOR, NOT

void and32_r_r(uint8_t reg1, uint8_t reg2)
{
}

void and32_r_i(uint8_t reg, uint32_t imm32)
{
}

void and32_r_m(uint8_t reg1, uint8_t reg2)
{
}

void and32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
}

void and32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
}

// OR operations: OR r,r r,m r,i r,mr

void or32_r_r(uint8_t reg1, uint8_t reg2)
{
}

void or32_r_i(uint8_t reg, uint32_t imm32)
{
}

void or32_r_m(uint8_t reg1, uint8_t reg2)
{
}

void or32_r_mi(uint8_t reg1, uint8_t reg2, uint32_t offset)
{
}

void or32_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
}

// NOT operations: NOT r, NOT [r], NOT [r+disp], NOT [r_base+index]

void not32_r(uint8_t reg)
{
}

void not32_m(uint8_t reg)
{
}

void not32_mi(uint8_t reg, uint32_t offset)
{
}

void not32_mr(uint8_t reg_base, uint8_t reg_index)
{
}

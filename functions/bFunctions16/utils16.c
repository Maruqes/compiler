#include "bFunctions16.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
#include "../bFunctions32/bFunctions32.h"

void cmp16_r_r(uint8_t reg1, uint8_t reg2)
{
}

void cmp16_r_i(uint8_t reg1, uint16_t imm16)
{
}

void cmp16_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);
}

void cmp16_r_mi(uint8_t reg1, uint8_t reg2, uint16_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);
}

void lshfit16(uint8_t reg, uint8_t imm)
{
}

void rshfit16(uint8_t reg, uint8_t imm)
{
}

void lshfit16_reg(uint8_t reg1)
{
}

void rshfit16_reg(uint8_t reg1)
{
}

// Logical operations: AND, OR, XOR, NOT

void and16_r_r(uint8_t reg1, uint8_t reg2)
{
}

void and16_r_i(uint8_t reg, uint16_t imm16)
{
}

void and16_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);
}

void and16_r_mi(uint8_t reg1, uint8_t reg2, uint16_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);
}

void and16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

    if (reg3 == REG_RSP) // ESP RSP ALL SAME SHIT ITS ALL "4"
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }
}

// OR operations: OR r,r r,m r,i r,mr

void or16_r_r(uint8_t reg1, uint8_t reg2)
{
}

void or16_r_i(uint8_t reg, uint16_t imm16)
{
}

void or16_r_m(uint8_t reg1, uint8_t reg2)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);
}

void or16_r_mi(uint8_t reg1, uint8_t reg2, uint16_t offset)
{
    cant_use_rx((uint8_t[]){reg1, reg2}, 2);
}

void or16_r_mr(uint8_t reg1, uint8_t reg2, uint8_t reg3)
{
    cant_use_rx((uint8_t[]){reg1, reg2, reg3}, 3);

    if (reg3 == REG_RSP)
    {
        fprintf(stderr, "Error: Cannot use ESP as an index register.\n");
        exit(EXIT_FAILURE);
    }
}

// NOT operations: NOT r, NOT [r], NOT [r+disp], NOT [r_base+index]

void not16_r(uint8_t reg)
{
}

void not16_m(uint8_t reg)
{
    cant_use_rx((uint8_t[]){reg}, 1);
}

void not16_mi(uint8_t reg, uint16_t offset)
{
    cant_use_rx((uint8_t[]){reg}, 1);
}

void not16_mr(uint8_t reg_base, uint8_t reg_index)
{
    cant_use_rx((uint8_t[]){reg_base, reg_index}, 2);
}

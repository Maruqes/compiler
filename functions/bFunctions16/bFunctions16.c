#include "bFunctions16.h"
#include "../functions.h"

/*
i-immediate
r-register
m-memory

functions:

mov16 r, i  :D
mov16 r, m  :D
mov16 r, mi :D
mov16 r, mr :D
mov16 r, r  :D

mov16 m, i  :D
mov16 m, r  :D

mov16 mi, i :D
mov16 mi, r :D

mov16 mr, i
mov16 mr, r
*/

// Function to move immediate value into a 16-bit register
void mov16_r_i(uint8_t reg_code, uint16_t value)
{
    // Under development
}

// Function to move from memory to a 16-bit register
void mov16_r_m(uint8_t reg, uint8_t mem_reg)
{
    // Under development
}

// Function to move from memory with immediate offset to a 16-bit register
void mov16_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
    // Under development
}

// Function to move from memory with register offset to a 16-bit register
void mov16_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset)
{
    // Under development
}

// Function to move between 16-bit registers
void mov16_r_r(uint8_t reg1, uint8_t reg2)
{
    // Under development
}

// Function to move immediate to memory
void mov16_m_i(uint8_t reg1, uint16_t value)
{
    // Under development
}

// Function to move register to memory
void mov16_m_r(uint8_t reg1, uint8_t reg2)
{
    // Under development
}

// Function to move immediate to memory with immediate offset
void mov16_mi_i(uint8_t reg, int32_t offset, uint16_t value)
{
    // Under development
}

// Function to move register to memory with immediate offset
void mov16_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    // Under development
}

// Function to move immediate to memory with register offset
void mov16_mr_i(uint8_t reg, uint8_t reg2, uint16_t value)
{
    // Under development
}

// Function to move register to memory with register offset
void mov16_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
    // Under development
}
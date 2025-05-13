#ifndef FUNCTIONS_16_H
#define FUNCTIONS_16_H

#include <stdint.h>

#define REG_AX 0x0
#define REG_CX 0x1
#define REG_DX 0x2
#define REG_BX 0x3
// #define REG_SP 0x4
// #define REG_BP 0x5
#define REG_SI 0x6
#define REG_DI 0x7

// Function prototypes for moving immediate values into 16-bit registers
void mov_reg16(uint8_t reg_code, uint16_t value);

// Function prototypes for moving data between memory and 16-bit registers
// mov reg16, [var + var_offset]
void mov_reg16_from_var(uint8_t reg_code, char *symbol, int var_offset);

// mov [var + var_offset], reg16
void mov_var_from_reg16(uint8_t reg_code, char *symbol, int var_offset);

void push_reg16(uint8_t reg_code);


void pop_reg16(uint8_t reg_code);


void cmp_reg16(uint8_t reg1, uint8_t reg2);

#endif // FUNCTIONS_16_H

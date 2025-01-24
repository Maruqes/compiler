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
void mov_ax(uint16_t value);
void mov_bx(uint16_t value);
void mov_cx(uint16_t value);
void mov_dx(uint16_t value);
void mov_si(uint16_t value);
void mov_di(uint16_t value);

// Function prototypes for moving data between memory and 16-bit registers
// mov reg16, [var + var_offset]
void mov_reg16_from_var(uint8_t reg_code, char *symbol, int var_offset);
void mov_ax_from_var(char *symbol, int var_offset);
void mov_bx_from_var(char *symbol, int var_offset);
void mov_cx_from_var(char *symbol, int var_offset);
void mov_dx_from_var(char *symbol, int var_offset);
void mov_si_from_var(char *symbol, int var_offset);
void mov_di_from_var(char *symbol, int var_offset);

// mov [var + var_offset], reg16
void mov_var_from_reg16(uint8_t reg_code, char *symbol, int var_offset);
void mov_var_from_ax(char *symbol, int var_offset);
void mov_var_from_bx(char *symbol, int var_offset);
void mov_var_from_cx(char *symbol, int var_offset);
void mov_var_from_dx(char *symbol, int var_offset);
void mov_var_from_si(char *symbol, int var_offset);
void mov_var_from_di(char *symbol, int var_offset);

void push_reg16(uint8_t reg_code);
void push_ax();
void push_bx();
void push_cx();
void push_dx();
void push_si();
void push_di();

void pop_reg16(uint8_t reg_code);
void pop_ax();
void pop_bx();
void pop_cx();
void pop_dx();
void pop_si();
void pop_di();

void cmp_reg16(uint8_t reg1, uint8_t reg2);

#endif // FUNCTIONS_16_H

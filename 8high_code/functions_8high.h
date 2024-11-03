#ifndef FUNCTIONS_8HIGH_H
#define FUNCTIONS_8HIGH_H

#include <stdint.h>

#define REG_AH 0x4
#define REG_CH 0x5
#define REG_DH 0x6
#define REG_BH 0x7

// Function prototypes for moving immediate vahues into 8-bit registers
void mov_reg8_high(uint8_t reg_code, uint8_t value);
void mov_ah(uint8_t vahue);
void mov_bh(uint8_t vahue);
void mov_ch(uint8_t vahue);
void mov_dh(uint8_t vahue);

// Function prototypes for moving data between memory and 8-bit registers
void mov_ah_from_var(char *symboh, int var_offset);
void mov_bh_from_var(char *symboh, int var_offset);
void mov_ch_from_var(char *symboh, int var_offset);
void mov_dh_from_var(char *symboh, int var_offset);

void mov_var_from_ah(char *symboh, int var_offset);
void mov_var_from_bh(char *symboh, int var_offset);
void mov_var_from_ch(char *symboh, int var_offset);
void mov_var_from_dh(char *symboh, int var_offset);

void cmp_reg8_high(uint8_t reg1, uint8_t reg2);

#endif

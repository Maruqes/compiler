#ifndef FUNCTIONS_8LOW_H
#define FUNCTIONS_8LOW_H

#include <stdint.h>

#define REG_AL 0x0
#define REG_CL 0x1
#define REG_DL 0x2
#define REG_BL 0x3

// Function prototypes for moving immediate values into 8-bit registers
void mov_reg8(uint8_t reg_code, uint8_t value);
void mov_al(uint8_t value);
void mov_bl(uint8_t value);
void mov_cl(uint8_t value);
void mov_dl(uint8_t value);

// Function prototypes for moving data between memory and 8-bit registers
// mov reg, [var + var_offset]
void mov_reg8_from_var(uint8_t reg_code, char *symbol, int var_offset);
void mov_al_from_var(char *symbol, int var_offset);
void mov_bl_from_var(char *symbol, int var_offset);
void mov_cl_from_var(char *symbol, int var_offset);
void mov_dl_from_var(char *symbol, int var_offset);

// mov [var + var_offset], reg8
void mov_var_from_reg8(uint8_t reg_code, char *symbol, int var_offset);
void mov_var_from_al(char *symbol, int var_offset);
void mov_var_from_bl(char *symbol, int var_offset);
void mov_var_from_cl(char *symbol, int var_offset);
void mov_var_from_dl(char *symbol, int var_offset);

void mov_var_from_reg8_offREG(uint8_t reg_code, uint8_t *symbol, uint8_t reg2);

void cmp_reg8(uint8_t reg1, uint8_t reg2);

#endif

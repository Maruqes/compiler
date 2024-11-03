// functions_16.c

#include "functions_8high.h"
#include "../functions/functions.h" // Adjust the path as needed
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../8low_code/functions_8low.h"

// Functions to move immediate values into high 8-bit registers
void mov_ah(uint8_t value) { mov_reg8(REG_AH, value); }
void mov_ch(uint8_t value) { mov_reg8(REG_CH, value); }
void mov_dh(uint8_t value) { mov_reg8(REG_DH, value); }
void mov_bh(uint8_t value) { mov_reg8(REG_BH, value); }

// Helper functions for high 8-bit registers
void mov_ah_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_AH, symbol, var_offset); }
void mov_ch_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_CH, symbol, var_offset); }
void mov_dh_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_DH, symbol, var_offset); }
void mov_bh_from_var(char *symbol, int var_offset) { mov_reg8_from_var(REG_BH, symbol, var_offset); }

// Helper functions for high 8-bit registers
void mov_var_from_ah(char *symbol, int var_offset) { mov_var_from_reg8(REG_AH, symbol, var_offset); }
void mov_var_from_ch(char *symbol, int var_offset) { mov_var_from_reg8(REG_CH, symbol, var_offset); }
void mov_var_from_dh(char *symbol, int var_offset) { mov_var_from_reg8(REG_DH, symbol, var_offset); }
void mov_var_from_bh(char *symbol, int var_offset) { mov_var_from_reg8(REG_BH, symbol, var_offset); }

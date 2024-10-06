#ifndef FUNCTIONS_16_H
#define FUNCTIONS_16_H

#include <stdint.h>

// Function prototypes for moving immediate values into 16-bit registers
void mov_ax(uint16_t value);
void mov_bx(uint16_t value);
void mov_cx(uint16_t value);
void mov_dx(uint16_t value);
void mov_si(uint16_t value);
void mov_di(uint16_t value);

// Function prototypes for moving symbol addresses into 16-bit registers
void mov_ax_symbol_address(char *symbol_name);
void mov_bx_symbol_address(char *symbol_name);
void mov_cx_symbol_address(char *symbol_name);
void mov_dx_symbol_address(char *symbol_name);
void mov_si_symbol_address(char *symbol_name);
void mov_di_symbol_address(char *symbol_name);

// Function prototypes for moving data between memory and 16-bit registers
void mov_ax_from_var(char *symbol, int var_offset);
void mov_bx_from_var(char *symbol, int var_offset);
void mov_cx_from_var(char *symbol, int var_offset);
void mov_dx_from_var(char *symbol, int var_offset);
void mov_si_from_var(char *symbol, int var_offset);
void mov_di_from_var(char *symbol, int var_offset);

void mov_var_from_ax(char *symbol, int var_offset);
void mov_var_from_bx(char *symbol, int var_offset);
void mov_var_from_cx(char *symbol, int var_offset);
void mov_var_from_dx(char *symbol, int var_offset);
void mov_var_from_si(char *symbol, int var_offset);
void mov_var_from_di(char *symbol, int var_offset);

#endif // FUNCTIONS_16_H

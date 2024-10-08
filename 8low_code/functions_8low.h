#ifndef FUNCTIONS_8LOW_H
#define FUNCTIONS_8LOW_H

#include <stdint.h>

// Function prototypes for moving immediate values into 8-bit registers
void mov_al(uint8_t value);
void mov_bl(uint8_t value);
void mov_cl(uint8_t value);
void mov_dl(uint8_t value);

// Function prototypes for moving data between memory and 8-bit registers
void mov_al_from_var(char *symbol, int var_offset);
void mov_bl_from_var(char *symbol, int var_offset);
void mov_cl_from_var(char *symbol, int var_offset);
void mov_dl_from_var(char *symbol, int var_offset);

void mov_var_from_al(char *symbol, int var_offset);
void mov_var_from_bl(char *symbol, int var_offset);
void mov_var_from_cl(char *symbol, int var_offset);
void mov_var_from_dl(char *symbol, int var_offset);



#endif

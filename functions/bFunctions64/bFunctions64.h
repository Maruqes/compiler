#ifndef BFUNCTIONS64_H
#define BFUNCTIONS64_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// Move to/from registers
void mov64_r_i(uint8_t reg_code, uint64_t value);
void mov64_r_m(uint8_t reg, uint8_t mem_reg);
void mov64_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset);
void mov64_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset);
void mov64_r_r(uint8_t reg1, uint8_t reg2);

// Move to memory
void mov64_m_i(uint8_t reg1, uint64_t value);
void mov64_m_r(uint8_t reg1, uint8_t reg2);
void mov64_mi_i(uint8_t reg, int32_t offset, uint64_t value);
void mov64_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2);
void mov64_mr_i(uint8_t reg, uint8_t reg2, uint64_t value);
void mov64_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3);

// Function prototypes for system calls
void syscall_instruction();

#endif

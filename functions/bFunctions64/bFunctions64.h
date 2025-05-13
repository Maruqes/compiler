#ifndef BFUNCTIONS64_H
#define BFUNCTIONS64_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// General Purpose Registers (0-7)
#define REG_RAX 0x0
#define REG_RCX 0x1
#define REG_RDX 0x2
#define REG_RBX 0x3
#define REG_RSP 0x4
#define REG_RBP 0x5
#define REG_RSI 0x6
#define REG_RDI 0x7

// Extended Registers (8-15)
#define REG_R8 0x8
#define REG_R9 0x9
#define REG_R10 0xA
#define REG_R11 0xB
#define REG_R12 0xC
#define REG_R13 0xD
#define REG_R14 0xE
#define REG_R15 0xF


// Special Registers (Use with caution, not for general mov instructions)
#define REG_RIP 0x10
#define REG_RFLAGS 0x11

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

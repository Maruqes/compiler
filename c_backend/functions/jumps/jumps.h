
#ifndef JUMPS_H
#define JUMPS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define JE_OPCODE 0x84	// 0F 84
#define JNE_OPCODE 0x85 // 0F 85
#define JG_OPCODE 0x8F	// 0F 8F
#define JL_OPCODE 0x8C	// 0F 8C
#define JGE_OPCODE 0x8D // 0F 8D
#define JLE_OPCODE 0x8E // 0F 8E
#define JA_OPCODE 0x87	// 0F 87
#define JB_OPCODE 0x82	// 0F 82
#define JAE_OPCODE 0x83 // 0F 83
#define JBE_OPCODE 0x86 // 0F 86

// Jump function prototypes
void jump_short(uint8_t offset);
void jump_near(uint32_t offset);
void jump_reg(uint8_t reg);
void jmp(char *name);
void jcc(char *name, uint8_t condition);
void call(char *name);
void call_raw_address(uint8_t reg);

// Comprehensive test functions
void test_jump_short(void);
void test_jump_near(void);
void test_jump_reg(void);
void test_jmp_labels(void);
void test_jcc_conditions(void);
void funcao_teste_all_jumps(void);

// labels
void create_label(char *name);
void fix_all_labels();
void add_label_jump(char *name, char *addr, uint16_t instruction_size);
void create_label_reference(uint8_t reg, char *name);
#endif

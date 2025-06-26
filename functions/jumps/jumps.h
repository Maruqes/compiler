
#ifndef JUMPS_H
#define JUMPS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// Jump function prototypes
void jump_short(uint8_t offset);
void jump_near(uint32_t offset);
void jump_reg(uint8_t reg);

// Test function prototypes
void funcao_teste_jump_short(void);
void funcao_teste_jump_near(void);
void funcao_teste_jump_reg(void);

#endif

#ifndef BFUNCTIONS64_H
#define BFUNCTIONS64_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>



void mov64_r_i(uint8_t reg_code, uint64_t value);
void mov64_r_r(uint8_t reg1, uint8_t reg2);

// Function prototypes for system calls
void syscall_instruction();

#endif

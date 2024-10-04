#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

extern size_t custom_code_size;

void mov_eax(int __fd, uint32_t code);
void mov_ebx(int __fd, uint32_t code);
void mov_ecx(int __fd, uint32_t code);
void mov_edx(int __fd, uint32_t code);
void our_syscall(int __fd);
void pre_mov_eax();
void pre_mov_ebx();
void pre_mov_ecx();
void pre_mov_edx();
void pre_syscall();

#endif // FUNCTIONS_H
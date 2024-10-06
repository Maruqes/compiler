#ifndef PUSH_POP_H
#define PUSH_POP_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void push_reg(int reg);
void push_eax();
void push_ebx();
void push_ecx();
void push_edx();
void push_esi();
void push_edi();
void push_symbol_address(char *symbol_name);
void push_symbol(char *symbol_name);

void pop_reg(int reg);
void pop_eax();
void pop_ebx();
void pop_ecx();
void pop_edx();
void pop_esi();
void pop_edi();
void pop_symbol(char *symbol_name);

#endif
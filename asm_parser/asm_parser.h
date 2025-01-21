#ifndef ASM_PARSER_H
#define ASM_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int convert_string_to_reg(char *reg);
int *asm_get_number(char **str, int pos, uint8_t reg);

// 16
int parse_movs16(FILE *file, char **tokens);

int parse_movs8(FILE *file, char **tokens);
#endif // ASM_PARSER_H
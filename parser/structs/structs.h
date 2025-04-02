#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void parse_struct(FILE *file);
void init_structs();
int parse_struct_contructors(FILE *file, char *token, int reg);
int parse_struct_variables(FILE *file, char *token, int reg);
#endif
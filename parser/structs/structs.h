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
void init_struct_var_types();
int parse_struct_constructor(FILE *file, char *token);
int parse_struct_variables(FILE *file, char *token);
#endif
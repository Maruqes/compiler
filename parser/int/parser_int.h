#ifndef PARSER_INT_H
#define PARSER_INT_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void parse_int_setter(FILE *file, char *token);
void parse_create_int(FILE *file, char *token);
void parse_set_value_in_the_pointer_address(FILE *file);

#endif
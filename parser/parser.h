#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define NUMBER_OF_PAGES 1

void start_parsing(char *filename);
char *get_token(FILE *fp);
void go_back_x_char(int x, FILE *file);
void parse_block(FILE *file);
void parse_create_function(FILE *file);
void parse_create_return(FILE *file);
#endif
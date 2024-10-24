#ifndef PARSER_HELP_H
#define PARSER_HELP_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct Function_struct
{
    char *name;
    char *return_type;
} Function_struct;

int is_a_function(char *token);
void add_function(char *name, char *return_type);

extern Function_struct *functions;
extern uint32_t functions_count;

#endif
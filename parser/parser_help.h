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

void parse_functions(FILE *file, char *token);
int is_a_function(char *token);
void add_function(char *name);
void free_functions();
int checkFuncType(char *type);
int parse_inside_functions(FILE *file, char *token, uint8_t reg);

int get_check_free_semicolon(FILE *f);

extern Function_struct *functions;
extern uint32_t functions_count;

void set_current_scope(char *scope);
char *get_current_scope();
void free_current_scope();

// parser functions
void parse_data_types(FILE *file, char *token, uint8_t reg);
// returns in EAX
void parse_after_equal(FILE *file);
void parse_inside_bracets_for_arrays(FILE *file);

// string functions
void parse_string_array_value_setter(FILE *file, char *arr_var_name);

void parse_asm_function(FILE *file);
int is_valid_number(const char *str);
int checkFuncType(char *type);

void parse_float(FILE *fp, char *token, uint8_t reg);
#endif
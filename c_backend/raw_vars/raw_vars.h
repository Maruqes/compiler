
#ifndef RAW_VARS_H
#define RAW_VARS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

// Function to add a string constant to the data section
void add_string_constant(char *name, char *content);

// Function to create a reference to a string constant that will be loaded into a register
void create_variable_reference(char *variable_name, uint8_t register_code);

// Function to write all string constants to the output file
void write_string_constants_to_file(int file_descriptor);

// Function to resolve all variable references with their absolute addresses
void resolve_variable_addresses(uint64_t data_section_start_address);
z
#endif

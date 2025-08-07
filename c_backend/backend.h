#ifndef BACKEND_H
#define BACKEND_H

#define BASE_ADDRESS 0x400000;

#include <stdint.h>
#include <stdlib.h>

// Core functions and definitions
#include "functions/functions.h"

// 8-bit operations
#include "functions/bFunctions8/bFunctions8.h"

// 16-bit operations
#include "functions/bFunctions16/bFunctions16.h"

// 32-bit operations
#include "functions/bFunctions32/bFunctions32.h"

// 64-bit operations
#include "functions/bFunctions64/bFunctions64.h"

// Jump operations
#include "functions/jumps/jumps.h"

// Raw variables and string constants
#include "raw_vars/raw_vars.h"

int write_elf();

#endif // BACKEND_H

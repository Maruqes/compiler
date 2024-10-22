#ifndef JUMPS_H
#define JUMPS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void call(char *name);
void jmp(char *name);
void jump_if_equal(char *name);
void jump_if_not_equal(char *name);
void jump_if_greater(char *name);
void jump_if_greater_or_equal(char *name);
void jump_if_less(char *name);
void jump_if_less_or_equal(char *name);

#endif // JUMPS_H
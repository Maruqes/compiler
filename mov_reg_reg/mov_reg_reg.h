#ifndef MOV_REG_REG_H
#define MOV_REG_REG_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void mov_eax_ebx();
void mov_eax_ecx();
void mov_eax_edx();
void mov_eax_esi();
void mov_eax_edi();

void mov_ebx_eax();
void mov_ebx_ecx();
void mov_ebx_edx();
void mov_ebx_esi();
void mov_ebx_edi();

void mov_ecx_eax();
void mov_ecx_ebx();
void mov_ecx_edx();
void mov_ecx_esi();
void mov_ecx_edi();

void mov_edx_eax();
void mov_edx_ebx();
void mov_edx_ecx();
void mov_edx_esi();
void mov_edx_edi();

void mov_esi_eax();
void mov_esi_ebx();
void mov_esi_ecx();
void mov_esi_edx();
void mov_esi_edi();

void mov_edi_eax();
void mov_edi_ebx();
void mov_edi_ecx();
void mov_edi_edx();
void mov_edi_esi();

#endif // MOV_REG_REG_H
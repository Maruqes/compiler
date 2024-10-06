#include "mov_reg_reg.h"
#include "functions.h"
#include "strings.h"

void mov_reg_reg(int b1, int b2)
{
    char *opcode_bytes = malloc(2);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = b1;
    opcode_bytes[1] = b2;

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

// mov eax, reg
void mov_eax_ebx()
{
    mov_reg_reg(0x89, 0xD8);
}

void mov_eax_ecx()
{
    mov_reg_reg(0x89, 0xC1);
}

void mov_eax_edx()
{
    mov_reg_reg(0x89, 0xC2);
}

void mov_eax_esi()
{
    mov_reg_reg(0x89, 0xC6);
}

void mov_eax_edi()
{
    mov_reg_reg(0x89, 0xC7);
}

// mov ebx, reg
void mov_ebx_eax()
{
    mov_reg_reg(0x89, 0xC3);
}

void mov_ebx_ecx()
{
    mov_reg_reg(0x89, 0xCB);
}

void mov_ebx_edx()
{
    mov_reg_reg(0x89, 0xD3);
}

void mov_ebx_esi()
{
    mov_reg_reg(0x89, 0xF3);
}

void mov_ebx_edi()
{
    mov_reg_reg(0x89, 0xF7);
}

// mov ecx, reg
void mov_ecx_eax()
{
    mov_reg_reg(0x89, 0xC1);
}

void mov_ecx_ebx()
{
    mov_reg_reg(0x89, 0xD9);
}

void mov_ecx_edx()
{
    mov_reg_reg(0x89, 0xCA);
}

void mov_ecx_esi()
{
    mov_reg_reg(0x89, 0xF1);
}

void mov_ecx_edi()
{
    mov_reg_reg(0x89, 0xF9);
}

// mov edx, reg
void mov_edx_eax()
{
    mov_reg_reg(0x89, 0xD0);
}

void mov_edx_ebx()
{
    mov_reg_reg(0x89, 0xD3);
}

void mov_edx_ecx()
{
    mov_reg_reg(0x89, 0xD1);
}

void mov_edx_esi()
{
    mov_reg_reg(0x89, 0xF2);
}

void mov_edx_edi()
{
    mov_reg_reg(0x89, 0xF7);
}

// mov esi, reg
void mov_esi_eax()
{
    mov_reg_reg(0x89, 0xF0);
}

void mov_esi_ebx()
{
    mov_reg_reg(0x89, 0xF3);
}

void mov_esi_ecx()
{
    mov_reg_reg(0x89, 0xF1);
}

void mov_esi_edx()
{
    mov_reg_reg(0x89, 0xF2);
}

void mov_esi_edi()
{
    mov_reg_reg(0x89, 0xF7);
}

// mov edi, reg
void mov_edi_eax()
{
    mov_reg_reg(0x89, 0xF8);
}

void mov_edi_ebx()
{
    mov_reg_reg(0x89, 0xFB);
}

void mov_edi_ecx()
{
    mov_reg_reg(0x89, 0xF9);
}

void mov_edi_edx()
{
    mov_reg_reg(0x89, 0xFA);
}

void mov_edi_esi()
{
    mov_reg_reg(0x89, 0xF6);
}
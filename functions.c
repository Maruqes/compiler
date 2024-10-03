#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

size_t custom_code_size = 0;

void mov_eax(int __fd, uint32_t code)
{
    char mov_eax_template[5];
    mov_eax_template[0] = 0xB8; // Opcode for 'mov eax, imm32'

    memcpy(&mov_eax_template[1], &code, sizeof(code));

    write(__fd, mov_eax_template, 5);
}

void mov_ebx(int __fd, uint32_t code)
{
    char mov_ebx_template[5];
    mov_ebx_template[0] = 0xBB; // Opcode for 'mov ebx, imm32'
    memcpy(&mov_ebx_template[1], &code, sizeof(code));
    write(__fd, mov_ebx_template, 5);
}

void mov_ecx(int __fd, uint32_t code)
{
    char mov_ecx_template[5];
    mov_ecx_template[0] = 0xB9; // Opcode for 'mov ecx, imm32'
    memcpy(&mov_ecx_template[1], &code, sizeof(code));
    write(__fd, mov_ecx_template, 5);
}

void mov_edx(int __fd, uint32_t code)
{
    char mov_edx_template[5];
    mov_edx_template[0] = 0xBA; // Opcode for 'mov edx, imm32'
    memcpy(&mov_edx_template[1], &code, sizeof(code));
    write(__fd, mov_edx_template, 5);
}

void our_syscall(int __fd)
{
    unsigned char syscall_code[] = {0xCD, 0x80};
    write(__fd, syscall_code, 2);
}

void pre_mov_eax()
{
    custom_code_size += 5;
}

void pre_mov_ebx()
{
    custom_code_size += 5;
}

void pre_mov_ecx()
{
    custom_code_size += 5;
}

void pre_mov_edx()
{
    custom_code_size += 5;
}

void pre_syscall()
{
    custom_code_size += 2;
}
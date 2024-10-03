#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "functions.c"

// ELF header structure for 32-bit executable
struct Elf32_Ehdr
{
    unsigned char e_ident[16]; // ELF identification
    uint16_t e_type;           // Object file type
    uint16_t e_machine;        // Machine type
    uint32_t e_version;        // Object file version
    uint32_t e_entry;          // Entry point address
    uint32_t e_phoff;          // Program header offset
    uint32_t e_shoff;          // Section header offset (unused)
    uint32_t e_flags;          // Processor-specific flags
    uint16_t e_ehsize;         // ELF header size
    uint16_t e_phentsize;      // Program header entry size
    uint16_t e_phnum;          // Number of program header entries
    uint16_t e_shentsize;      // Section header entry size (unused)
    uint16_t e_shnum;          // Number of section header entries (unused)
    uint16_t e_shstrndx;       // Section header string table index (unused)
};

// Program header structure for loadable segments
struct Elf32_Phdr
{
    uint32_t p_type;   // Type of segment
    uint32_t p_offset; // Offset in file
    uint32_t p_vaddr;  // Virtual address in memory
    uint32_t p_paddr;  // Physical address (unused)
    uint32_t p_filesz; // Size of segment in file
    uint32_t p_memsz;  // Size of segment in memory
    uint32_t p_flags;  // Segment attributes
    uint32_t p_align;  // Alignment
};

// Machine code (x86): read input into buff and write it back

unsigned char code[] = {
    // sys_read syscall
    0xB8, 0x03, 0x00, 0x00, 0x00, // mov eax, 3          ; sys_read
    0xBB, 0x00, 0x00, 0x00, 0x00, // mov ebx, 0          ; fd = 0 (stdin)
    0xB9, 0x00, 0x00, 0x00, 0x00, // mov ecx, buff       ; buff address (placeholder)
    0xBA, 0x0C, 0x00, 0x00, 0x00, // mov edx, 12         ; bytes to read
    0xCD, 0x80,                   // int 0x80            ; syscall

    0xA3, 0x00, 0x00, 0x00, 0x00, // mov [text_address], eax ; save input

    0xB8, 0x04, 0x00, 0x00, 0x00,       // mov eax, 4          ; sys_write
    0xBB, 0x01, 0x00, 0x00, 0x00,       // mov ebx, 1          ; fd = 1 (stdout)
    0xB9, 0x00, 0x00, 0x00, 0x00,       // mov ecx, msg        ; msg address (placeholder)
    0x8B, 0x15, 0x00, 0x00, 0x00, 0x00, // mov edx, 12         ; msg length
    0xCD, 0x80,                         // int 0x80            ; syscall

    0xB8, 0x04, 0x00, 0x00, 0x00, // mov eax, 4          ; sys_write
    0xBB, 0x01, 0x00, 0x00, 0x00, // mov ebx, 1          ; fd = 1 (stdout)
    0xB9, 0x00, 0x00, 0x00, 0x00, // mov ecx, msg        ; msg address (placeholder)
    0xBA, 0x0C, 0x00, 0x00, 0x00, // mov edx, 12         ; msg length
    0xCD, 0x80,                   // int 0x80            ; syscall
};

unsigned char buff[12] = {0};
unsigned char text_random[12] = "12345678901\n";
unsigned char buff_len[4];

#define BASE_ADDRESS 0x08048000 // Common base address for 32-bit executables

int main()
{
    pre_mov_eax();
    pre_mov_ebx();
    pre_syscall();

    pre_mov_eax();
    pre_mov_ebx();
    pre_mov_ecx();
    pre_mov_edx();
    pre_syscall();

    // ELF header
    struct Elf32_Ehdr ehdr = {0};
    memcpy(ehdr.e_ident, "\x7f"
                         "ELF",
           4);           // ELF Magic number
    ehdr.e_ident[4] = 1; // 32-bit
    ehdr.e_ident[5] = 1; // Little-endian
    ehdr.e_ident[6] = 1; // ELF version
    ehdr.e_type = 2;     // Executable file
    ehdr.e_machine = 3;  // x86 architecture
    ehdr.e_version = 1;  // ELF version

    // Calculate offsets and addresses
    size_t code_offset = sizeof(struct Elf32_Ehdr) + sizeof(struct Elf32_Phdr);
    size_t code_size = sizeof(code);

    size_t final_vars_size = sizeof(buff) + sizeof(text_random);

    ehdr.e_entry = BASE_ADDRESS + code_offset;    // Entry point (where code starts)
    ehdr.e_phoff = sizeof(struct Elf32_Ehdr);     // Program header offset
    ehdr.e_ehsize = sizeof(struct Elf32_Ehdr);    // ELF header size
    ehdr.e_phentsize = sizeof(struct Elf32_Phdr); // Program header entry size
    ehdr.e_phnum = 1;                             // Number of program headers

    // Program header (describing the executable segment)
    struct Elf32_Phdr phdr = {0};
    phdr.p_type = 1;                                                // PT_LOAD (Loadable segment)
    phdr.p_offset = code_offset;                                    // Offset of the code in the file
    phdr.p_vaddr = BASE_ADDRESS + code_offset;                      // Virtual address to load code
    phdr.p_paddr = phdr.p_vaddr;                                    // Physical address (not used)
    phdr.p_filesz = code_size + final_vars_size + custom_code_size; // Size of code + buffer in the file
    phdr.p_memsz = code_size + final_vars_size + custom_code_size;  // Size of code + buffer in memory
    phdr.p_flags = 7;                                               // Read + Write + Execute permissions
    phdr.p_align = 0x1000;                                          // Alignment (page size)

    // Dynamically calculate the buffer address
    uint32_t buff_address = phdr.p_vaddr + code_size + custom_code_size;
    uint32_t text_address = phdr.p_vaddr + code_size + sizeof(buff) + custom_code_size;
    uint32_t buff_len_address = phdr.p_vaddr + code_size + sizeof(buff) + sizeof(text_random) + custom_code_size;

    // Insert buffer address into the machine code
    memcpy(&code[11], &buff_address, sizeof(buff_address));
    memcpy(&code[23], &buff_len_address, sizeof(buff_len_address));

    memcpy(&code[38], &buff_address, sizeof(buff_address));
    memcpy(&code[44], &buff_len_address, sizeof(buff_len_address));

    memcpy(&code[61], &text_address, sizeof(text_address));

    // Write the ELF file
    int fd = open("hello_elf_32", O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    write(fd, &ehdr, sizeof(ehdr)); // Write ELF header
    write(fd, &phdr, sizeof(phdr)); // Write program header

    write(fd, code, code_size); // Write machine code

    mov_eax(fd, 0x04);
    mov_ebx(fd, 0x01);
    mov_ecx(fd, text_address);
    mov_edx(fd, 0x0C);
    our_syscall(fd);

    mov_eax(fd, 0x01);
    mov_ebx(fd, 0x00);
    our_syscall(fd);

    write(fd, buff, sizeof(buff));               // Write buffer
    write(fd, text_random, sizeof(text_random)); // Write buffer
    write(fd, buff_len, sizeof(buff_len));       // Write buffer

    close(fd);

    return 0;
}

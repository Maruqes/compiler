#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "uint32.h"
#include "functions.h"
#include "strings.h"
#include "mov_reg_reg.h"
#include "push_pop.h"
#include "16_code/functions_16.h"
#include "8low_code/functions_8low.h"
#include "8high_code/functions_8high.h"
#include "jumps.h"

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

#define BASE_ADDRESS 0x08048000 // Common base address for 32-bit executables

void print(char *symbol_name)
{
    pusha();
    mov_eax(4);
    mov_ebx(0x01);
    mov_ecx_symbol_address(symbol_name, 0);
    mov_edx(22);
    our_syscall();
    popa();
}

int main()
{
    jmp("start");

    create_label("print_msg");
    print("msg3");
    mov_ebx(5);
    cmp_reg32(REG_EBX, REG_EAX);
    jump_if_not_equal("ab2");
    create_label("ab1");
    print("msg");
    ret();
    create_label("ab2");
    print("msg2");
    ret();

    create_label("start");

    mov_eax(5);
    call("print_msg");

    mov_eax(0x01); // sys_exit
    mov_ebx(0x00); // Exit code 0
    our_syscall();

    // Calculate code size
    add_custom_code_size();

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

    ehdr.e_entry = BASE_ADDRESS + code_offset;    // Entry point (where code starts)
    ehdr.e_phoff = sizeof(struct Elf32_Ehdr);     // Program header offset
    ehdr.e_ehsize = sizeof(struct Elf32_Ehdr);    // ELF header size
    ehdr.e_phentsize = sizeof(struct Elf32_Phdr); // Program header entry size
    ehdr.e_phnum = 1;                             // Number of program headers

    // Program header (describing the executable segment)
    struct Elf32_Phdr phdr = {0};
    phdr.p_type = 1;                           // PT_LOAD (Loadable segment)
    phdr.p_offset = code_offset;               // Offset of the code in the file
    phdr.p_vaddr = BASE_ADDRESS + code_offset; // Virtual address to load code
    phdr.p_paddr = phdr.p_vaddr;               // Physical address (not used)
    phdr.p_filesz = custom_code_size;          // Size of code + buffer in the file
    phdr.p_memsz = custom_code_size;           // Size of code + buffer in memory
    phdr.p_flags = 7;                          // Read + Write + Execute permissions
    phdr.p_align = 0x1000;                     // Alignment (page size)

    // All strings
    create_constant_string("msg", "Hello, World!\n", phdr.p_vaddr + custom_code_size + data_size);
    create_constant_string("msg2", "1234567890\n", phdr.p_vaddr + custom_code_size + data_size);
    create_constant_string("msg3", "asas mano bue malouco\n", phdr.p_vaddr + custom_code_size + data_size);

    // All uint32
    create_constant_uint32("uint0", 48, phdr.p_vaddr + custom_code_size + data_size);

    fix_label_addresses(phdr.p_vaddr);

    fixup_addresses();

    size_t total_data_size = 0;
    for (uint32_t i = 0; i < constant_string_count; i++)
    {
        total_data_size += strlen(constant_strings[i].var_value);
    }

    phdr.p_filesz = custom_code_size + data_size;
    phdr.p_memsz = custom_code_size + data_size;

    // Write the ELF file
    int fd = open("hello_elf_32", O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    write(fd, &ehdr, sizeof(ehdr)); // Write ELF header
    write(fd, &phdr, sizeof(phdr)); // Write program header

    write_all_custom_code(fd);
    write_all_string_constants(fd);
    write_all_uint32_constants(fd);

    close(fd);

    cleanup();

    return 0;
}

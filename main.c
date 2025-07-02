#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "functions/functions.h"
#include "functions/bFunctions64/bFunctions64.h"
#include "functions/bFunctions32/bFunctions32.h"
#include "functions/bFunctions16/bFunctions16.h"
#include "functions/bFunctions8/bFunctions8.h"
#include "functions/jumps/jumps.h"
#include "raw_vars/raw_vars.h"

#define BASE_ADDRESS 0x400000 // Common base address for 64-bit executables
#define ELF_MAGIC "\x7f" \
                  "ELF"
#define ELF_CLASS_64 2 // 64-bit class
#define ELF_CLASS_32 1 // 32-bit class (kept for reference)
#define ELF_DATA_LITTLE_ENDIAN 1
#define ELF_VERSION_CURRENT 1
#define ELF_TYPE_EXECUTABLE 2
#define ELF_MACHINE_X86_64 62 // x86-64 architecture
#define ELF_MACHINE_X86 3     // 32-bit x86 architecture (kept for reference)
#define ELF_PT_LOAD 1
#define ELF_FLAGS_RWX 7
#define ELF_ALIGN_PAGE 0x1000

// ELF header structure for 32-bit executable (kept for reference)
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

// Program header structure for 32-bit loadable segments (kept for reference)
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

// ELF header structure for 64-bit executable
struct Elf64_Ehdr
{
    unsigned char e_ident[16]; // ELF identification
    uint16_t e_type;           // Object file type
    uint16_t e_machine;        // Machine type
    uint32_t e_version;        // Object file version
    uint64_t e_entry;          // Entry point address
    uint64_t e_phoff;          // Program header offset
    uint64_t e_shoff;          // Section header offset (unused)
    uint32_t e_flags;          // Processor-specific flags
    uint16_t e_ehsize;         // ELF header size
    uint16_t e_phentsize;      // Program header entry size
    uint16_t e_phnum;          // Number of program header entries
    uint16_t e_shentsize;      // Section header entry size (unused)
    uint16_t e_shnum;          // Number of section header entries (unused)
    uint16_t e_shstrndx;       // Section header string table index (unused)
};

// Program header structure for 64-bit loadable segments
struct Elf64_Phdr
{
    uint32_t p_type;   // Type of segment
    uint32_t p_flags;  // Segment attributes
    uint64_t p_offset; // Offset in file
    uint64_t p_vaddr;  // Virtual address in memory
    uint64_t p_paddr;  // Physical address (unused)
    uint64_t p_filesz; // Size of segment in file
    uint64_t p_memsz;  // Size of segment in memory
    uint64_t p_align;  // Alignment
};

void cleanup()
{
    for (uint32_t i = 0; i < op_codes_array_size; i++)
    {
        free(op_codes_array[i].code);
    }
    free(op_codes_array);

    // Free fixups
    for (uint32_t i = 0; i < fixups_array_size; i++)
    {
        free(fixups_array[i].symbol_name);
    }
    free(fixups_array);

    // Free jump array
    for (uint32_t i = 0; i < jump_array_size; i++)
    {
        free(jump_array[i].var_name);
    }
    free(jump_array);
}

void init_elf_header(struct Elf64_Ehdr *ehdr, size_t code_offset)
{
    memset(ehdr, 0, sizeof(struct Elf64_Ehdr));
    memcpy(ehdr->e_ident, ELF_MAGIC, 4);
    ehdr->e_ident[4] = ELF_CLASS_64; // 64-bit class
    ehdr->e_ident[5] = ELF_DATA_LITTLE_ENDIAN;
    ehdr->e_ident[6] = ELF_VERSION_CURRENT;
    ehdr->e_type = ELF_TYPE_EXECUTABLE;
    ehdr->e_machine = ELF_MACHINE_X86_64; // x86-64 architecture
    ehdr->e_version = ELF_VERSION_CURRENT;
    ehdr->e_entry = BASE_ADDRESS + code_offset;
    ehdr->e_phoff = sizeof(struct Elf64_Ehdr);
    ehdr->e_ehsize = sizeof(struct Elf64_Ehdr);
    ehdr->e_phentsize = sizeof(struct Elf64_Phdr);
    ehdr->e_phnum = 1;
}

void init_program_header(struct Elf64_Phdr *phdr, size_t code_offset, size_t custom_code_size, size_t data_size)
{
    memset(phdr, 0, sizeof(struct Elf64_Phdr));
    phdr->p_type = ELF_PT_LOAD;
    phdr->p_flags = ELF_FLAGS_RWX; // Note: in 64-bit, p_flags comes before p_offset
    phdr->p_offset = code_offset;
    phdr->p_vaddr = BASE_ADDRESS + code_offset;
    phdr->p_paddr = phdr->p_vaddr;
    phdr->p_filesz = custom_code_size + data_size;
    phdr->p_memsz = custom_code_size + data_size;
    phdr->p_align = ELF_ALIGN_PAGE;
}
int write_to_file(int fd, const void *buf, size_t count)
{
    if (write(fd, buf, count) != count)
    {
        perror("write");
        close(fd);
        return 1;
    }
    return 0;
}

// public Elf64_Phdr
struct Elf64_Phdr phdrPublic;

int create_elf()
{
    uint64_t custom_code_size = get_current_code_size();

    // Read the custom code from the input file
    // ELF header
    struct Elf64_Ehdr ehdr;
    size_t code_offset = sizeof(struct Elf64_Ehdr) + sizeof(struct Elf64_Phdr);
    init_elf_header(&ehdr, code_offset);

    // Program header
    struct Elf64_Phdr phdr;
    init_program_header(&phdr, code_offset, custom_code_size, data_size);

    // Write the ELF file
    int fd = open("hello_elf_64", O_CREAT | O_WRONLY | O_TRUNC, 0755);
    if (fd < 0)
    {
        perror("open");
        return 1;
    }

    if (write_to_file(fd, &ehdr, sizeof(ehdr)) ||
        write_to_file(fd, &phdr, sizeof(phdr)))
    {
        return 1;
    }

    off_t current_pos = lseek(fd, 0, SEEK_CUR);
    if (current_pos == (off_t)-1)
    {
        perror("lseek");
        close(fd);
        return 1;
    }
    printf("Current seek position: %ld\n", (long)current_pos);
    memcpy(&phdrPublic, &phdr, sizeof(phdr));
    return fd;
}

void printHello()
{
    mov64_r_i(REG_RAX, 1); // syscall number for write
    mov64_r_i(REG_RDI, 1); // file descriptor 1 (stdout)
    create_variable_reference("string3", REG_RSI);
    mov64_r_i(REG_RDX, 23); // length of the string
    syscall_instruction();  // invoke syscall
}

void write_code()
{
    funcao_teste_and64_r_r();
    funcao_teste_and64_r_i();
    funcao_teste_and64_r_m();
    funcao_teste_and64_r_mi();
    funcao_teste_and64_r_mr();

    create_label("exit_label");
    mov64_r_i(REG_RAX, 0x3c);
    mov64_r_i(REG_RDI, 21);
    syscall_instruction();
}

int main(int argc, char *argv[])
{
    // proximos passos:
    /*
    desenvolver 64/32/16/8 r/m/i
    criar labels e jumps
    conditionais em 64/32/16/8
    criar readonly parts
    desenvolver 64/32/16/8 push/pop
    desenvolver 64/32/16/8 arithmetic
    desenvolver 64/32/16/8 logic(ands/ors/xors)
    desenvolver 64/32/16/8 shifts

    ter cuidado com sp/bp
    */

    write_code();

    add_string_constant("string1", "Hello, World!\n");
    add_string_constant("string2", "AII, MEUU!\n");
    add_string_constant("string3", "DEUSSS, JESUS CHRIST!\n");

    int fd = create_elf();

    fix_all_labels();
    resolve_variable_addresses(phdrPublic.p_vaddr + get_current_code_size());

    // needs to be this order
    write_all_custom_code(fd);
    write_string_constants_to_file(fd);
    close(fd);

    cleanup();

    return 0;
}

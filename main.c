#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include "functions/functions.h"
#include "functions/bFunctions64/bFunctions64.h"

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

void write_code()
{
    mov64_r_i(REG_RAX, 0x1111111111111111ULL);
    mov64_r_i(REG_RBX, 0x2222222222222222ULL);
    mov64_r_i(REG_RCX, 0x3333333333333333ULL);
    mov64_r_i(REG_RDX, 0x4444444444444444ULL);
    mov64_r_i(REG_RSI, 0x5555555555555555ULL);
    mov64_r_i(REG_RDI, 0x6666666666666666ULL);
    mov64_r_i(REG_RSP, 0x7777777777777777ULL);
    mov64_r_i(REG_RBP, 0x8888888888888888ULL);
    mov64_r_i(REG_R8, 0x9999999999999999ULL);
    mov64_r_i(REG_R9, 0xAAAAAAAAAAAAAAAAULL);
    mov64_r_i(REG_R10, 0xBBBBBBBBBBBBBBBBULL);
    mov64_r_i(REG_R11, 0xCCCCCCCCCCCCCCCCULL);
    mov64_r_i(REG_R12, 0xDDDDDDDDDDDDDDDDULL);
    mov64_r_i(REG_R13, 0xEEEEEEEEEEEEEEEEULL);
    mov64_r_i(REG_R14, 0xFFFFFFFFFFFFFFFFULL);
    mov64_r_i(REG_R15, 0xABCDEFABCDEFABCDULL);

    mov64_r_r(REG_RAX, REG_R12);
    mov64_r_r(REG_R13, REG_RBX);
    mov64_r_r(REG_R14, REG_RSI);
    mov64_r_r(REG_RDX, REG_R15);
    mov64_r_r(REG_R8, REG_RSP);
    mov64_r_r(REG_R9, REG_RBP);
    mov64_r_r(REG_R10, REG_RDI);
    mov64_r_r(REG_R11, REG_RAX);
    mov64_r_r(REG_R12, REG_RCX);
    mov64_r_r(REG_R15, REG_R8);

    mov64_r_m(REG_RAX, REG_RSP); // base = rsp → SIB obrigatório
    mov64_r_m(REG_R12, REG_R12); // base = r12 → SIB obrigatório
    mov64_r_m(REG_RBX, REG_RBP); // base = rbp → mod=01 + disp8
    mov64_r_m(REG_R14, REG_R13); // base = r13 → mod=01 + disp8
    mov64_r_m(REG_R10, REG_R8);  // high regs
    mov64_r_m(REG_R9, REG_R9);   // high regs
    mov64_r_m(REG_R13, REG_R14);
    mov64_r_m(REG_R15, REG_R15);

    mov64_r_mi(REG_RAX, REG_RSP, 0x10);   // base = rsp → SIB + disp
    mov64_r_mi(REG_R12, REG_RBP, 0x00);   // base = rbp → mod=01 + disp8 = 0
    mov64_r_mi(REG_R9, REG_R13, 0x00);    // base = r13 → igual ao de cima
    mov64_r_mi(REG_R14, REG_R12, 0x40);   // SIB + high regs
    mov64_r_mi(REG_R8, REG_R14, -0x8);    // disp negativo
    mov64_r_mi(REG_R10, REG_R11, 0x1000); // disp32 longo

    mov64_r_mr(REG_RAX, REG_RBX, REG_RCX); // low+low
    mov64_r_mr(REG_R8, REG_R12, REG_R13);  // high+high → REX completo
    mov64_r_mr(REG_R9, REG_RSP, REG_RAX);  // base = rsp → obriga SIB
    mov64_r_mr(REG_R10, REG_R12, REG_RSP); // index = rsp (interpreta como sem index)
    mov64_r_mr(REG_R11, REG_RAX, REG_RSP); // index = rsp → ignora
    mov64_r_mr(REG_R11, REG_RAX, REG_R12); // index = rsp → ignora
    mov64_r_mr(REG_R15, REG_R13, REG_R14); // REX.R, REX.X, REX.B todos ativos
    mov64_r_mr(REG_RAX, REG_RBX, REG_RBX); // base = index

    mov64_m_i32(REG_RAX, 0x11223344);
    mov64_m_i32(REG_RBX, 0x55667788);
    mov64_m_i32(REG_RSP, 0x99AABBCC); // SIB obrigatório
    mov64_m_i32(REG_RBP, 0xDDDDDDDD); // obriga disp8
    mov64_m_i32(REG_R13, 0xEEEEEEEE); // obriga disp8
    mov64_m_i32(REG_R8, 0x12345678);  // high reg
    mov64_m_i32(REG_R9, 0x89ABCDEF);
    mov64_m_i32(REG_R12, 0xDEADBEEF); // SIB obrigatório
    mov64_m_i32(REG_R14, 0xCAFEBABE);
    mov64_m_i32(REG_R15, 0xFACEFACE);

    mov64_r_i(REG_RAX, 0x3c);
    mov64_r_i(REG_RDI, 21);
    syscall_instruction();
}

int main(int argc, char *argv[])
{
    // proximos passos:
    /*
    criar labels e jumps
    conditionais em 64/32/16/8
    criar readonly parts
    desenvolver 64/32/16/8 r/m/i
    desenvolver 64/32/16/8 push/pop
    desenvolver 64/32/16/8 arithmetic
    desenvolver 64/32/16/8 logic(ands/ors/xors)

    ter cuidado com sp/bp
    */

    write_code();
    uint64_t custom_code_size = add_custom_code_size();

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

    write_all_custom_code(fd);

    close(fd);

    cleanup();

    return 0;
}

#include "functions.h"
#include "../types/strings.h"
#include "../types/uint32.h"
#include "../arithmetic/arithmetic.h"

size_t custom_code_size = 0;

OpCode *op_codes_array = NULL;    // Dynamic array of opcodes
uint32_t op_codes_array_size = 0; // Number of opcodes stored

Fixup *fixups_array = NULL;
uint32_t fixups_array_size = 0;

Jump_struct *jump_array = NULL;
uint32_t jump_array_size = 0;

void add_fixup(int index, char *symbol_name, int offset, uint32_t var_offset, uint32_t jump_offset)
{
    uint32_t addr = 0;
    for (uint32_t i = 0; i < op_codes_array_size; i++)
    {
        addr += op_codes_array[i].size;
    }

    // Record the fixup
    fixups_array = realloc(fixups_array, (fixups_array_size + 1) * sizeof(Fixup));
    if (fixups_array == NULL)
    {
        perror("Failed to reallocate memory for fixups_array");
        exit(EXIT_FAILURE);
    }

    Fixup new_fixup;
    new_fixup.jump_offset = jump_offset;
    new_fixup.opcode_index = index;
    new_fixup.offset_in_opcode = offset; // Offset where the address should be inserted
    new_fixup.var_offset = var_offset;
    new_fixup.symbol_name = strdup(symbol_name); // Remember to free this later
    new_fixup.code_offset = addr;
    fixups_array[fixups_array_size] = new_fixup;
    fixups_array_size++;
}

void fixup_addresses()
{
    for (uint32_t i = 0; i < fixups_array_size; i++)
    {
        Fixup fixup = fixups_array[i];
        // Find the symbol's address
        uint32_t symbol_address = 0;
        for (uint32_t j = 0; j < constant_string_count; j++)
        {
            if (strcmp(fixup.symbol_name, constant_strings[j].var_name) == 0)
            {
                symbol_address = constant_strings[j].var_address;
                break;
            }
        }

        // uint8_t enc = 0;
        // for (uint32_t j = 0; j < constant_string_count && !enc; enc = !strcmp(fixup.symbol_name, constant_strings[j++].var_name))
        // {
        //     symbol_address = constant_strings[j].var_address;
        // }

        for (uint32_t j = 0; j < constant_uint32_count; j++)
        {
            if (strcmp(fixup.symbol_name, constant_uint32s[j].var_name) == 0)
            {
                symbol_address = constant_uint32s[j].var_address;
                break;
            }
        }

        for (uint32_t j = 0; j < jump_array_size; j++)
        {
            if (strcmp(fixup.symbol_name, jump_array[j].var_name) == 0)
            {
                int32_t displacement = jump_array[j].var_address - (fixup.code_offset) - fixup.jump_offset;

                printf("\nvar address: %d\n", jump_array[j].var_address);
                printf("opcode index: %d\n", fixup.code_offset);
                printf("displacement: %d\n\n", displacement);

                symbol_address = displacement;
                break;
            }
        }

        for (uint32_t j = 0; j < constant_uint32_arr_count; j++)
        {
            if (strcmp(fixup.symbol_name, constant_uint32_arrs[j].var_name) == 0)
            {
                symbol_address = constant_uint32_arrs[j].var_address;
                break;
            }
        }

        if (symbol_address == 0)
        {
            fprintf(stderr, "Undefined symbol: %s\n", fixup.symbol_name);
            exit(EXIT_FAILURE);
        }

        symbol_address += fixup.var_offset;
        memcpy(&op_codes_array[fixup.opcode_index].code[fixup.offset_in_opcode], &symbol_address, sizeof(symbol_address));
    }
}

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
}

// MOV code FUNCTIONS

// Function to move immediate value into a 32-bit register
void mov_reg32(uint8_t reg_code, uint32_t value)
{
    char *opcode_bytes = malloc(5); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xB8 + reg_code; // Opcode for 'mov reg32, imm32'

    memcpy(&opcode_bytes[1], &value, sizeof(value)); // Copy the 4-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 5; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the opcode array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov_eax(uint32_t code)
{
    mov_reg32(REG_EAX, code);
}

void mov_ebx(uint32_t code)
{
    mov_reg32(REG_EBX, code);
}

void mov_ecx(uint32_t code)
{
    mov_reg32(REG_ECX, code);
}

void mov_edx(uint32_t code)
{
    mov_reg32(REG_EDX, code);
}

void mov_esi(uint32_t code)
{
    mov_reg32(REG_ESI, code);
}

void mov_edi(uint32_t code)
{
    mov_reg32(REG_EDI, code);
}

// MOV SYMBOL FUNCTIONS

void mov_reg32_symbol_address(uint8_t reg_code, char *symbol_name, int var_offset)
{
    char *opcode_bytes = malloc(5); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xB8 + reg_code; // Opcode for 'mov reg32, imm32'

    memset(&opcode_bytes[1], 0, 4); // Placeholder for address

    OpCode new_opcode;
    new_opcode.size = 5; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add fixup for the symbol address
    add_fixup(op_codes_array_size, symbol_name, 1, var_offset, 0); // Offset is 1 due to opcode

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov_eax_symbol_address(char *symbol_name, int var_offset)
{
    mov_reg32_symbol_address(REG_EAX, symbol_name, var_offset);
}

void mov_ebx_symbol_address(char *symbol_name, int var_offset)
{
    mov_reg32_symbol_address(REG_EBX, symbol_name, var_offset);
}

void mov_ecx_symbol_address(char *symbol_name, int var_offset)
{
    mov_reg32_symbol_address(REG_ECX, symbol_name, var_offset);
}

void mov_edx_symbol_address(char *symbol_name, int var_offset)
{
    mov_reg32_symbol_address(REG_EDX, symbol_name, var_offset);
}

void mov_esi_symbol_address(char *symbol_name, int var_offset)
{
    mov_reg32_symbol_address(REG_ESI, symbol_name, var_offset);
}

void mov_edi_symbol_address(char *symbol_name, int var_offset)
{
    mov_reg32_symbol_address(REG_EDI, symbol_name, var_offset);
}

// SYSCALL FUNCTIONS
void our_syscall()
{
    unsigned char *syscall_code = malloc(2);
    if (syscall_code == NULL)
    {
        perror("Failed to allocate memory for syscall_code");
        exit(EXIT_FAILURE);
    }
    syscall_code[0] = 0xCD;
    syscall_code[1] = 0x80;

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 2; // Correct size
    new_opcode.code = syscall_code;

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

void add_custom_code_size()
{
    for (uint32_t i = 0; i < op_codes_array_size; i++)
    {
        custom_code_size += op_codes_array[i].size;
    }
}

void write_all_custom_code(int __fd)
{
    for (int i = 0; i < op_codes_array_size; i++)
    {
        write(__fd, op_codes_array[i].code, op_codes_array[i].size);
    }
}

void mov_var_from_reg32(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(6); // 1-byte opcode + 1-byte ModR/M + 4-byte address
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x89;                   // Opcode for 'mov r/m32, reg32'
    opcode_bytes[1] = (reg_code << 3) | 0x05; // ModR/M byte for 'disp32' addressing mode

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    OpCode new_opcode;
    new_opcode.size = 6; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 2, var_offset, 0); // Offset is 2 due to opcode and ModR/M

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov_var_from_eax(char *symbol, int var_offset)
{
    mov_var_from_reg32(REG_EAX, symbol, var_offset);
}

void mov_var_from_ebx(char *symbol, int var_offset)
{
    mov_var_from_reg32(REG_EBX, symbol, var_offset);
}

void mov_var_from_ecx(char *symbol, int var_offset)
{
    mov_var_from_reg32(REG_ECX, symbol, var_offset);
}

void mov_var_from_edx(char *symbol, int var_offset)
{
    mov_var_from_reg32(REG_EDX, symbol, var_offset);
}

void mov_var_from_esi(char *symbol, int var_offset)
{
    mov_var_from_reg32(REG_ESI, symbol, var_offset);
}

void mov_var_from_edi(char *symbol, int var_offset)
{
    mov_var_from_reg32(REG_EDI, symbol, var_offset);
}

void mov_reg32_from_var(uint8_t reg_code, char *symbol, int var_offset)
{
    char *opcode_bytes = malloc(6); // 1-byte opcode + 1-byte ModR/M + 4-byte address
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x8B;                   // Opcode for 'mov reg32, r/m32'
    opcode_bytes[1] = (reg_code << 3) | 0x05; // ModR/M byte for 'disp32' addressing mode

    memset(&opcode_bytes[2], 0, 4); // Placeholder for address

    OpCode new_opcode;
    new_opcode.size = 6; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add fixup for the address
    add_fixup(op_codes_array_size, symbol, 2, var_offset, 0); // Offset is 2 due to opcode and ModR/M

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov_eax_from_var(char *symbol, int var_offset)
{
    mov_reg32_from_var(REG_EAX, symbol, var_offset);
}

void mov_ebx_from_var(char *symbol, int var_offset)
{
    mov_reg32_from_var(REG_EBX, symbol, var_offset);
}

void mov_ecx_from_var(char *symbol, int var_offset)
{
    mov_reg32_from_var(REG_ECX, symbol, var_offset);
}

void mov_edx_from_var(char *symbol, int var_offset)
{
    mov_reg32_from_var(REG_EDX, symbol, var_offset);
}

void mov_esi_from_var(char *symbol, int var_offset)
{
    mov_reg32_from_var(REG_ESI, symbol, var_offset);
}

void mov_edi_from_var(char *symbol, int var_offset)
{
    mov_reg32_from_var(REG_EDI, symbol, var_offset);
}

void inc_eax()
{
    inc_reg32(REG_EAX);
}

void inc_ebx()
{
    inc_reg32(REG_EBX);
}

void inc_ecx()
{
    inc_reg32(REG_ECX);
}

void inc_edx()
{
    inc_reg32(REG_EDX);
}

void inc_esi()
{
    inc_reg32(REG_ESI);
}

void inc_edi()
{
    inc_reg32(REG_EDI);
}

void create_label(char *name)
{
    uint32_t addr = 0;
    for (uint32_t i = 0; i < op_codes_array_size; i++)
    {
        addr += op_codes_array[i].size;
    }

    jump_array = realloc(jump_array, sizeof(Jump_struct) * (jump_array_size + 1));
    jump_array[jump_array_size].var_name = malloc(strlen(name) + 1);
    if (!jump_array[jump_array_size].var_name)
    {
        perror("Failed to allocate memory for jump_array[jump_array_size].var_name");
        exit(EXIT_FAILURE);
    }
    strcpy(jump_array[jump_array_size].var_name, name);
    jump_array[jump_array_size].var_address = addr;
    jump_array_size++;
    printf("Label %s at %d\n", name, jump_array[jump_array_size - 1].var_address);
}

void fix_label_addresses(uint32_t fix_size)
{
    for (uint32_t i = 0; i < jump_array_size; i++)
    {
        jump_array[i].fix_addr = fix_size;
    }
}

void cmp_reg32(uint8_t reg1, uint8_t reg2)
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x39;
    opcode_bytes[1] = 0xC0 + reg1 + (reg2 * 8);

    OpCode new_opcode;
    new_opcode.size = 2; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void ret()
{
    char *opcode_bytes = malloc(1);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xC3;

    OpCode new_opcode;
    new_opcode.size = 1; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void nop()
{
    char *opcode_bytes = malloc(1);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x90;

    OpCode new_opcode;
    new_opcode.size = 1; // Total instruction size
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

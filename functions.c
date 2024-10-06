#include "functions.h"
#include "strings.h"

size_t custom_code_size = 0;

OpCode *op_codes_array = NULL;    // Dynamic array of opcodes
uint32_t op_codes_array_size = 0; // Number of opcodes stored

Fixup *fixups_array = NULL;
uint32_t fixups_array_size = 0;

void add_fixup(int index, char *symbol_name)
{
    // Record the fixup
    fixups_array = realloc(fixups_array, (fixups_array_size + 1) * sizeof(Fixup));
    if (fixups_array == NULL)
    {
        perror("Failed to reallocate memory for fixups_array");
        exit(EXIT_FAILURE);
    }

    Fixup new_fixup;
    new_fixup.opcode_index = op_codes_array_size;
    new_fixup.offset_in_opcode = 1;              // Offset where the address should be inserted
    new_fixup.symbol_name = strdup(symbol_name); // Remember to free this later
    fixups_array[fixups_array_size] = new_fixup;
    fixups_array_size++;
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
        if (symbol_address == 0)
        {
            fprintf(stderr, "Undefined symbol: %s\n", fixup.symbol_name);
            exit(EXIT_FAILURE);
        }
        // Insert the address into the opcode
        memcpy(&op_codes_array[fixup.opcode_index].code[fixup.offset_in_opcode], &symbol_address, sizeof(symbol_address));
    }
}

// MOV code FUNCTIONS
void mov_eax(uint32_t code)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xB8;
    memcpy(&opcode_bytes[1], &code, sizeof(code));

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov_ebx(uint32_t code)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xBB;
    memcpy(&opcode_bytes[1], &code, sizeof(code)); // Copy the immediate value

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov_ecx(uint32_t code)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xB9;

    memcpy(&opcode_bytes[1], &code, sizeof(code)); // Copy the immediate value

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov_edx(uint32_t code)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xBA;

    memcpy(&opcode_bytes[1], &code, sizeof(code)); // Copy the immediate value

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov_esi(uint32_t code)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xBE;

    memcpy(&opcode_bytes[1], &code, sizeof(code)); // Copy the immediate value

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov_edi(uint32_t code)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xBF;

    memcpy(&opcode_bytes[1], &code, sizeof(code)); // Copy the immediate value

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 5;
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

// MOV SYMBOL FUNCTIONS
void mov_eax_symbol(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name);
    mov_eax(0);
}

void mov_ebx_symbol(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name);
    mov_ebx(0);
}

void mov_ecx_symbol(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name);
    mov_ecx(0);
}

void mov_edx_symbol(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name);
    mov_edx(0);
}

void mov_esi_symbol(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name);
    mov_esi(0);
}

void mov_edi_symbol(char *symbol_name)
{
    add_fixup(op_codes_array_size, symbol_name);
    mov_edi(0);
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

void mov_var_from_eax(uint32_t code)
{
    char *opcode_bytes = malloc(5);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0xA3; // Opcode for 'mov [var], eax'

    memcpy(&opcode_bytes[1], &code, sizeof(code)); // Copy the immediate value

    // Create an OpCode structure
    OpCode new_opcode;
    new_opcode.size = 5;
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

void mov_var_from_ebx(uint32_t code)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x1D;
    memcpy(&opcode_bytes[2], &code, sizeof(code));

    OpCode new_opcode;
    new_opcode.size = 6; // 1-byte opcode + 1-byte ModRM + 4-byte address
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

void mov_var_from_ecx(uint32_t code)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x0D;
    memcpy(&opcode_bytes[2], &code, sizeof(code));

    OpCode new_opcode;
    new_opcode.size = 6;
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

void mov_var_from_edx(uint32_t code)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x15;
    memcpy(&opcode_bytes[2], &code, sizeof(code));

    OpCode new_opcode;
    new_opcode.size = 6;
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

void mov_var_from_esi(uint32_t code)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x35;
    memcpy(&opcode_bytes[2], &code, sizeof(code));

    OpCode new_opcode;
    new_opcode.size = 6;
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

void mov_var_from_edi(uint32_t code)
{
    char *opcode_bytes = malloc(6);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0] = 0x89;
    opcode_bytes[1] = 0x3D;
    memcpy(&opcode_bytes[2], &code, sizeof(code));

    OpCode new_opcode;
    new_opcode.size = 6;
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
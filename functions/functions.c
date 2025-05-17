#include "functions.h"
#include "bFunctions32/bFunctions32.h"
#include "bFunctions64/bFunctions64.h"

size_t data_size = 0; // Size of data section

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

        for (uint32_t j = 0; j < jump_array_size; j++)
        {
            if (strcmp(fixup.symbol_name, jump_array[j].var_name) == 0)
            {
                int32_t displacement = jump_array[j].var_address - (fixup.code_offset) - fixup.jump_offset;

                printf("\nvar address: %u\n", jump_array[j].var_address);
                printf("opcode index: %u\n", fixup.code_offset);
                printf("displacement: %d\n\n", displacement);

                symbol_address = displacement;
                break;
            }
        }

        if (symbol_address == 0)
        {
            if (strcmp(fixup.symbol_name, "main") == 0)
            {
                fprintf(stderr, "Function main not found\n");
                exit(EXIT_FAILURE);
            }
            fprintf(stderr, "Undefined symbol: %s\n", fixup.symbol_name);
            exit(EXIT_FAILURE);
        }

        symbol_address += fixup.var_offset;
        memcpy(&op_codes_array[fixup.opcode_index].code[fixup.offset_in_opcode], &symbol_address, sizeof(symbol_address));
    }
}

uint64_t add_custom_code_size()
{
    uint64_t custom_code_size = 0;
    for (uint32_t i = 0; i < op_codes_array_size; i++)
    {
        custom_code_size += op_codes_array[i].size;
    }
    return custom_code_size;
}

void write_all_custom_code(int __fd)
{
    for (int i = 0; i < op_codes_array_size; i++)
    {
        ssize_t bytes_written = write(__fd, op_codes_array[i].code, op_codes_array[i].size);
        if (bytes_written == -1)
        {
            perror("Failed to write opcode to file");
            exit(EXIT_FAILURE);
        }
        else if (bytes_written != op_codes_array[i].size)
        {
            fprintf(stderr, "Partial write occurred\n");
            exit(EXIT_FAILURE);
        }
    }
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

/*
mod = tipo de modo MOD_NO_DISP, MOD_1BYTE_DISP, MOD_4BYTE_DISP, MOD_REG_DIRECT
reg = registo de destino
rm = registo de origem ou registo de base
*/
void set_modrm(uint8_t *dst,
               uint8_t mod,
               uint8_t reg,
               uint8_t rm)
{
    *dst = mod | REG_FIELD(reg) | RM_FIELD(rm);
}

/*
scale = 0b00, 0b01, 0b10, 0b11  multiplicador de 1, 2, 4 ou 8
index = registo de indexação   [r12 + index * scale]
base = registo de base    registo base caso acima é o r12
*/
void set_sib(uint8_t *dst,
             uint8_t scale,
             uint8_t index,
             uint8_t base)
{
    *dst = scale | INDEX_FIELD(index) | BASE_FIELD(base);
}

/*
vais precisar de SIB base = RSP ou R12, ha mais casos mas acho que so vamos usar este

mod = tipo de modo MOD_NO_DISP, MOD_1BYTE_DISP, MOD_4BYTE_DISP, MOD_REG_DIRECT
reg_base = registo de base
usa_index = se o campo index vive no SIB ou não, (se usa o SIB que esta na funcao set_sib)
*/
int precisa_sib(uint8_t mod,
                uint8_t reg_base,
                int usa_index)
{
    /* Mod == 0b11 → endereçamento reg-reg, nunca há SIB */
    if (mod == MOD_REG_DIRECT)
        return 0;

    /* Se há índice, SIB é obrigatório (o campo index vive lá) */
    if (usa_index)
        return 1;

    /* Se o campo rm (3 bits baixos) vale 100b → SIB obrigatório */
    if ((reg_base & 0x7) == 0x4)
        return 1;

    /* Nenhuma condição apanhou → não precisas de SIB */
    return 0;
}

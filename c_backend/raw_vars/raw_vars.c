#include "raw_vars.h"
#include "../functions/bFunctions64/bFunctions64.h"
#include "../functions/functions.h"

typedef struct
{
    char *name;      // name of the string constant
    uint64_t offset; // offset within the data section
    char *content;   // actual string content
} StringConstant;

StringConstant *string_constants = NULL;
uint32_t string_constants_count = 0;
uint64_t data_section_size = 0;

void add_string_constant(char *name, char *content)
{
    if (!name || !content)
    {
        fprintf(stderr, "Error: NULL string passed to add_string_constant\n");
        return;
    }
    string_constants = realloc(string_constants, (string_constants_count + 1) * sizeof(StringConstant));
    if (!string_constants)
    {
        perror("Failed to allocate memory for string_constants");
        exit(EXIT_FAILURE);
    }

    string_constants[string_constants_count].name = strdup(name);
    if (!string_constants[string_constants_count].name)
    {
        perror("Failed to duplicate string for string_constants");
        exit(EXIT_FAILURE);
    }

    string_constants[string_constants_count].offset = data_section_size;

    string_constants[string_constants_count].content = strdup(content);
    if (!string_constants[string_constants_count].content)
    {
        perror("Failed to duplicate content string for string_constants");
        exit(EXIT_FAILURE);
    }

    string_constants_count++;

    size_t len = strlen(content);
    // +1 para o 0x00 que vamos escrever forçosamente
    if (data_section_size > UINT64_MAX - (len + 1))
    {
        fprintf(stderr, "Error: Data section size would overflow\n");
        exit(EXIT_FAILURE);
    }
    data_section_size += (len + 1);
}

void write_string_constants_to_file(int fd)
{
    for (uint32_t i = 0; i < string_constants_count; i++)
    {
        const char *s = string_constants[i].content;
        size_t len = strlen(s);

        // escreve o conteúdo "tal e qual"
        if (len > 0)
        {
            ssize_t w = write(fd, s, len);
            if (w == -1)
            {
                perror("Failed to write string content");
                exit(EXIT_FAILURE);
            }
            if ((size_t)w != len)
            {
                fprintf(stderr, "Partial write occurred for string constant '%s'\n", string_constants[i].name);
                exit(EXIT_FAILURE);
            }
        }

        // FORÇA 0x00 no fim da string no ficheiro (mesmo que o último char "visível" não seja 0)
        uint8_t zero = 0;
        ssize_t wz = write(fd, &zero, 1);
        printf("Wrote terminating NUL (0x00) for string constant '%s'\n", string_constants[i].name);
        if (wz == -1 || wz != 1)
        {
            perror("Failed to write terminating NUL (0x00)");
            exit(EXIT_FAILURE);
        }
    }
}

typedef struct
{
    char *variable_name;           // name of the string constant to reference
    char *instruction_address_ptr; // pointer to the immediate value in the mov instruction
} VariableReference;

VariableReference *variable_references = NULL;
uint32_t variable_references_count = 0;

void create_variable_reference(char *variable_name, uint8_t register_code)
{
    mov64_r_i(register_code, 0);
    char *immediate_address = &op_codes_array[op_codes_array_size - 1].code[2]; // gets the address of the placeholder written in the last opcode it will be substituted later

    variable_references = realloc(variable_references, (variable_references_count + 1) * sizeof(VariableReference));
    if (!variable_references)
    {
        perror("Failed to allocate memory for variable_references");
        exit(EXIT_FAILURE);
    }
    variable_references[variable_references_count].variable_name = strdup(variable_name);
    if (!variable_references[variable_references_count].variable_name)
    {
        perror("Failed to duplicate string for variable_references");
        exit(EXIT_FAILURE);
    }
    variable_references[variable_references_count].instruction_address_ptr = immediate_address;
    variable_references_count++;
}

void resolve_variable_addresses(uint64_t data_section_start_address)
{
    for (uint32_t i = 0; i < variable_references_count; i++)
    {
        bool found = false;
        for (uint32_t j = 0; j < string_constants_count; j++)
        {
            if (strcmp(variable_references[i].variable_name, string_constants[j].name) == 0)
            {
                // Calculate the absolute address: data section start + string offset
                uint64_t absolute_address = data_section_start_address + string_constants[j].offset;

                // Write the absolute address into the mov instruction's immediate field
                memcpy(variable_references[i].instruction_address_ptr, &absolute_address, sizeof(uint64_t));
                found = true;
                break; // Found match, no need to continue searching
            }
        }

        if (!found)
        {
            fprintf(stderr, "Error: Undefined string constant '%s'\n", variable_references[i].variable_name);
            exit(EXIT_FAILURE);
        }
    }
}
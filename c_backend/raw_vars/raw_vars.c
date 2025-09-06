#include "raw_vars.h"
#include "../functions/bFunctions64/bFunctions64.h"
#include "../functions/functions.h"

typedef struct
{
    char *name;        // symbol name
    uint64_t offset;   // offset in data section
    char *content;     // bytes (includes trailing NUL for strings)
    uint32_t size;     // number of bytes in content
    uint32_t align;    // alignment applied
    uint8_t is_string; // 1=string (content already has NUL), 0=raw global storage
} DataEntry;

DataEntry *data_entries = NULL;
uint32_t data_entries_count = 0;
uint64_t data_section_size = 0;

static inline uint64_t align_up(uint64_t v, uint64_t a) { return (v + a - 1) & ~(a - 1); }

static inline uint32_t pick_align(uint32_t sz)
{
    if (sz >= 8)
        return 8;
    if (sz >= 4)
        return 4;
    if (sz >= 2)
        return 2;
    return 1;
}

void add_string_constant(char *name, char *content)
{
    if (!name || !content)
    {
        fprintf(stderr, "Error: NULL string passed to add_string_constant\n");
        return;
    }
    data_entries = realloc(data_entries, (data_entries_count + 1) * sizeof(DataEntry));
    if (!data_entries)
    {
        perror("Failed to allocate memory for string_constants");
        exit(EXIT_FAILURE);
    }

    data_entries[data_entries_count].name = strdup(name);
    if (!data_entries[data_entries_count].name)
    {
        perror("Failed to duplicate string for string_constants");
        exit(EXIT_FAILURE);
    }
    size_t len = strlen(content);        // without forced NUL
    uint32_t full = (uint32_t)(len + 1); // include NUL terminator we will store
    uint32_t align = pick_align(full);
    data_section_size = align_up(data_section_size, align);
    data_entries[data_entries_count].offset = data_section_size;

    data_entries[data_entries_count].content = malloc(full);
    if (!data_entries[data_entries_count].content)
    {
        perror("Failed to duplicate content string for string_constants");
        exit(EXIT_FAILURE);
    }
    memcpy(data_entries[data_entries_count].content, content, len);
    data_entries[data_entries_count].content[len] = '\0';
    data_entries[data_entries_count].size = full;
    data_entries[data_entries_count].align = align;
    data_entries[data_entries_count].is_string = 1;

    if (data_section_size > UINT64_MAX - full)
    {
        fprintf(stderr, "Error: Data section size would overflow\n");
        exit(EXIT_FAILURE);
    }
    data_section_size += full;
    data_entries_count++;
}

// Raw zero-initialized global storage
void add_global_storage(char *name, uint32_t size)
{
    if (!name || size == 0)
    {
        fprintf(stderr, "Error: invalid args to add_global_storage\n");
        return;
    }
    data_entries = realloc(data_entries, (data_entries_count + 1) * sizeof(DataEntry));
    if (!data_entries)
    {
        perror("Failed to allocate memory for data_entries (globals)\n");
        exit(EXIT_FAILURE);
    }
    data_entries[data_entries_count].name = strdup(name);
    if (!data_entries[data_entries_count].name)
    {
        perror("Failed to duplicate global name\n");
        exit(EXIT_FAILURE);
    }
    uint32_t align = pick_align(size);
    data_section_size = align_up(data_section_size, align);
    data_entries[data_entries_count].offset = data_section_size;
    data_entries[data_entries_count].content = calloc(1, size);
    if (!data_entries[data_entries_count].content)
    {
        perror("Failed to allocate global storage content\n");
        exit(EXIT_FAILURE);
    }
    data_entries[data_entries_count].size = size;
    data_entries[data_entries_count].align = align;
    data_entries[data_entries_count].is_string = 0;
    if (data_section_size > UINT64_MAX - size)
    {
        fprintf(stderr, "Error: Data section size would overflow (globals)\n");
        exit(EXIT_FAILURE);
    }
    data_section_size += size;
    data_entries_count++;
}

void write_string_constants_to_file(int fd) // kept name for compatibility
{
    uint64_t file_pos = 0;
    for (uint32_t i = 0; i < data_entries_count; i++)
    {
        DataEntry *e = &data_entries[i];
        // pad if needed
        if (file_pos < e->offset)
        {
            uint64_t gap = e->offset - file_pos;
            const uint8_t zero_pad[16] = {0};
            while (gap)
            {
                size_t chunk = gap > 16 ? 16 : (size_t)gap;
                ssize_t w = write(fd, zero_pad, chunk);
                if (w != (ssize_t)chunk)
                {
                    perror("Failed to write alignment padding");
                    exit(EXIT_FAILURE);
                }
                gap -= chunk;
                file_pos += chunk;
            }
        }
        if (e->size)
        {
            ssize_t w = write(fd, e->content, e->size);
            if (w == -1 || (uint32_t)w != e->size)
            {
                fprintf(stderr, "Failed/partial write for entry '%s'\n", e->name);
                exit(EXIT_FAILURE);
            }
            file_pos += e->size;
        }
    }
}

uint64_t get_data_entries_size()
{
    return data_section_size;
}

typedef struct
{
    char *variable_name;   // name of the referenced symbol
    uint32_t opcode_index; // index into op_codes_array for the mov instruction
    uint8_t imm_offset;    // offset inside opcode for the imm64 (normally 2)
} VariableReference;

VariableReference *variable_references = NULL;
uint32_t variable_references_count = 0;

void create_variable_reference(char *variable_name, uint8_t register_code)
{
    mov64_r_i(register_code, 0); // emits: 48 B8+reg imm64
    if (op_codes_array_size == 0)
    {
        fprintf(stderr, "Internal error: no opcode emitted for variable reference\n");
        exit(EXIT_FAILURE);
    }
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
    variable_references[variable_references_count].opcode_index = op_codes_array_size - 1;
    variable_references[variable_references_count].imm_offset = 2; // skip REX + opcode
    variable_references_count++;
}

void resolve_variable_addresses(uint64_t data_section_start_address)
{
    for (uint32_t i = 0; i < variable_references_count; i++)
    {
        bool found = false;
        for (uint32_t j = 0; j < data_entries_count; j++)
        {
            if (strcmp(variable_references[i].variable_name, data_entries[j].name) == 0)
            {
                // Calculate the absolute address: data section start + string offset
                uint64_t absolute_address = data_section_start_address + data_entries[j].offset;

                // Resolve pointer fresh each time to avoid stale pointer after realloc
                uint32_t idx = variable_references[i].opcode_index;
                if (idx >= op_codes_array_size)
                {
                    fprintf(stderr, "Error: opcode index out of range while resolving '%s'\n", variable_references[i].variable_name);
                    exit(EXIT_FAILURE);
                }
                uint8_t off = variable_references[i].imm_offset;
                if (off + 8 > op_codes_array[idx].size)
                {
                    fprintf(stderr, "Error: immediate offset out of range for '%s'\n", variable_references[i].variable_name);
                    exit(EXIT_FAILURE);
                }
                char *imm_ptr = &op_codes_array[idx].code[off];
                memcpy(imm_ptr, &absolute_address, sizeof(uint64_t));
                found = true;
                break; // Found match, no need to continue searching
            }
        }

        if (!found)
        {
            fprintf(stderr, "Error: Undefined data symbol '%s'\n", variable_references[i].variable_name);
            exit(EXIT_FAILURE);
        }
    }
}
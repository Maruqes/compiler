#include "jumps.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"

typedef struct
{
    char *name;               // name of the label to jump
    uint64_t compile_address; // address of the jump instruction  hint: compiled program
    char *program_address;    // address of the instruction jump in the program hint: compiler program
    uint16_t intruction_size;
} Jump;

Jump *jumps = NULL;
uint32_t jumps_size = 0;

typedef struct
{
    char *name;
    uint64_t address;
} Label;

Label *labels = NULL;
uint32_t labels_size = 0;

void create_label(char *name)
{
    for (uint32_t i = 0; i < labels_size; i++)
    {
        if (strcmp(labels[i].name, name) == 0)
        {
            fprintf(stderr, "Label '%s' already exists\n", name);
            return;
        }
    }

    labels = realloc(labels, (labels_size + 1) * sizeof(Label));
    if (!labels)
    {
        perror("Failed to allocate memory for labels");
        exit(EXIT_FAILURE);
    }

    labels[labels_size].name = strdup(name);
    labels[labels_size].address = get_current_code_size();
    labels_size++;
}

void add_label_jump(char *name, char *addr, uint16_t instruction_size)
{
    jumps = realloc(jumps, (jumps_size + 1) * sizeof(Jump));
    if (!jumps)
    {
        perror("Failed to allocate memory for jumps");
        exit(EXIT_FAILURE);
    }

    jumps[jumps_size].name = strdup(name);
    jumps[jumps_size].program_address = addr;
    jumps[jumps_size].compile_address = get_current_code_size();
    jumps[jumps_size].intruction_size = instruction_size;
    jumps_size++;
}

void fix_all_labels()
{
    // loop all jumps and fix them
    for (uint32_t i = 0; i < jumps_size; i++)
    {
        int found = 0;
        for (uint32_t j = 0; j < labels_size; j++)
        {
            if (strcmp(jumps[i].name, labels[j].name) == 0)
            {
                int64_t diff = (int64_t)labels[j].address - (int64_t)(jumps[i].compile_address + jumps[i].intruction_size);
                if (diff < INT32_MIN || diff > INT32_MAX)
                {
                    fprintf(stderr, "Error: Address difference for label '%s' exceeds 32-bit rel32 jump range\n", jumps[i].name);
                    exit(EXIT_FAILURE);
                }

                int32_t rel32 = (int32_t)diff;
                memcpy((void *)jumps[i].program_address, &rel32, sizeof(int32_t));
                found = 1;
            }
        }
        if (!found)
        {
            fprintf(stderr, "Error: Jump to label '%s' not found\n", jumps[i].name);
            exit(EXIT_FAILURE);
        }
    }
}
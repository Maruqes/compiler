# Jump Instructions and Label Management

This document explains how jump instructions and labels are implemented in the compiler, including how forward references are resolved and addresses are fixed up during the compilation process.

## Overview

The jump and label system allows for control flow within the generated assembly code. It supports:
- **Forward jumps**: Jumping to labels that haven't been defined yet
- **Backward jumps**: Jumping to labels that have already been defined
- **Automatic address resolution**: The compiler handles the calculation of relative addresses

## Data Structures

### Jump Structure
```c
typedef struct {
    char *name;               // name of the label to jump to
    uint64_t compile_address; // address where the jump instruction is located
    char *program_address;    // pointer to the 4-byte displacement field in the instruction
} Jump;
```

### Label Structure
```c
typedef struct {
    char *name;      // name of the label
    uint64_t address; // absolute address where the label is placed
} Label;
```


### 1. Label Jump (`jmp`)
- **Opcode**: `0xE9` (near jump)
- **Size**: 5 bytes
- **Target**: Named label (resolved at compile time)
- **Formula**: `RIP = RIP + calculated displacement to label`

## Label and Jump Management Process

### Phase 1: Collection
During code generation, the compiler:

1. **Creates Labels**: When `createLabel(name)` is called:
   ```c
   labels[labels_size].name = strdup(name);
   labels[labels_size].address = get_current_code_size();
   ```
   - Stores the label name
   - Records the current code position as the label's address

2. **Records Jumps**: When `jmp(name)` is called:
   ```c
   opcode_bytes[0] = 0xE9;                    // Near jump opcode
   memset(&opcode_bytes[1], 0, 4);            // Placeholder for displacement
   add_label_jump(name, &opcode_bytes[1]);    // Register for later fixup
   ```
   - Generates the jump instruction with a placeholder displacement
   - Records the jump for later address resolution

### Phase 2: Address Resolution
After all code is generated, `fix_all_labels()` is called:

```c
void fix_all_labels() {
    for (uint32_t i = 0; i < jumps_size; i++) {
        for (uint32_t j = 0; j < labels_size; j++) {
            if (strcmp(jumps[i].name, labels[j].name) == 0) {
                // Calculate relative displacement
                int64_t diff = (int64_t)labels[j].address - 
                              (int64_t)(jumps[i].compile_address + 5);
                
                // Validate range
                if (diff < INT32_MIN || diff > INT32_MAX) {
                    fprintf(stderr, "Jump range exceeded\n");
                    exit(EXIT_FAILURE);
                }
                
                // Write the displacement
                int32_t rel32 = (int32_t)diff;
                memcpy(jumps[i].program_address, &rel32, sizeof(int32_t));
            }
        }
    }
}
```

## Address Calculation

The displacement calculation uses the formula:
```
displacement = target_address - (current_instruction_address + instruction_size)
```

Where:
- `target_address`: The address where the label is located
- `current_instruction_address`: The address of the jump instruction
- `instruction_size`: 5 bytes for near jumps (0xE9 + 4-byte displacement)

### Example:
```
Jump instruction at address: 0x1000
Label at address: 0x1050
Instruction size: 5 bytes

displacement = 0x1050 - (0x1000 + 5) = 0x1050 - 0x1005 = 0x4B
```

## Usage Example

```c
// Create a label at the current position
createLabel("loop_start");

// Generate some code...
mov64_r_i(REG_RAX, 42);
mov64_r_i(REG_RBX, 10);

// Jump back to the label
jmp("loop_start");

// At the end of compilation, fix all addresses
fix_all_labels();
```

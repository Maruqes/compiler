# String Constants and Variable References System

This document explains how string constants are managed in the compiler, including how they are stored in the data section and how references to them are resolved during the compilation process.

## Overview

The string constants system allows the compiler to:
- **Store string literals**: Define string constants that will be placed in the data section of the ELF file
- **Generate references**: Create instructions that load the addresses of these strings into registers
- **Resolve addresses**: Automatically calculate and patch the final memory addresses after the ELF layout is determined

This system works in two phases: **Collection Phase** (during code generation) and **Resolution Phase** (after memory layout is finalized).

## Data Structures

### StringConstant Structure
```c
typedef struct {
    char *name;         // symbolic name of the string (e.g., "string1")
    uint64_t offset;    // offset within the data section (relative to data start)
    char *content;      // actual string content (e.g., "Hello, World!\n")
} StringConstant;
```

### VariableReference Structure
```c
typedef struct {
    char *variable_name;           // name of the string constant to reference
    char *instruction_address_ptr; // pointer to the 8-byte immediate field in MOV instruction
} VariableReference;
```

## Memory Layout in ELF File

```
ELF File Layout:
┌─────────────────┐
│ ELF Header      │
├─────────────────┤
│ Program Header  │
├─────────────────┤ ← code_offset
│ Code Section    │
│ - mov rax, 1    │
│ - mov rdi, 1    │
│ - mov rsi, ADDR │ ← This ADDR gets patched
│ - mov rdx, 15   │
│ - syscall       │
├─────────────────┤ ← data_section_start_address
│ Data Section    │
│ "Hello, World!" │ ← String content
└─────────────────┘
```

## Step-by-Step Process

### Phase 1: Collection (During Code Generation)

#### Step 1: Generate Code with String References
When `create_variable_reference("string1", REG_RSI)` is called:

```c
void create_variable_reference(char *variable_name, uint8_t register_code)
{
    // 1. Generate a MOV instruction with placeholder address
    mov64_r_i(register_code, 0);  // Creates: MOV RSI, 0
    
    // 2. Get pointer to the immediate value (8 bytes starting at offset 2)
    char *immediate_address = &op_codes_array[op_codes_array_size - 1].code[2]; //[0]/[1] are bytes for the "mov" [2]starts the address
    
    // 3. Store this reference for later patching
    variable_references[i].variable_name = "string1";
    variable_references[i].instruction_address_ptr = immediate_address;
}
```

**Generated MOV Instruction Structure:**
```
Byte:  0    1    2    3    4    5    6    7    8    9
     [REX][OP][---- 64-bit Immediate Value ----]
     [48 ][B6][40][00][00][00][00][00][00][00]
              ^
              └─ instruction_address_ptr points here
```

#### Step 2: Register String Constants
When `add_string_constant("string1", "Hello, World!\n")` is called:

```c
void add_string_constant(char *name, char *content)
{
    string_constants[i].name = "string1";
    string_constants[i].offset = data_section_size;  // Current offset in data section
    string_constants[i].content = "Hello, World!\n";
    data_section_size += strlen(content);            // Update total data section size
}
```

**Data Section Layout Building:**
```
Offset  Content
0       "Hello, World!\n"     ← string1 at offset 0
14      (next string would go here)
```

### Phase 2: Resolution (After Memory Layout is Determined)

#### Step 3: Calculate Final Addresses
When `resolve_variable_addresses(data_section_start_address)` is called:

```c
void resolve_variable_addresses(uint64_t data_section_start_address)
{
    for each variable_reference:
        for each string_constant:
            if (names match):
                // Calculate absolute memory address
                absolute_address = data_section_start_address + string_constant.offset;
                
                // Patch the MOV instruction's immediate value
                memcpy(variable_reference.instruction_address_ptr, 
                       &absolute_address, sizeof(uint64_t));
}
```

**Address Calculation Example:**
```
data_section_start_address = 0x400078  (from ELF program header)
string_constant.offset     = 0         (first string in data section)
absolute_address          = 0x400078 + 0 = 0x400078
```

#### Step 4: Patch Instructions
The placeholder value `64` (0x0000000000000040) in the MOV instruction gets replaced:

**Before patching:**
```
MOV RSI, 0x0000000000000000    // no address
```

**After patching:**
```
MOV RSI, 0x0000000000400078   //adress with virtual address added from elf "phdrPublic.p_vaddr"
```

### Phase 3: File Generation

#### Step 5: Write Code Section
`write_all_custom_code(fd)` writes all generated instructions (now with correct addresses) to the file.

#### Step 6: Write Data Section
`write_string_constants_to_file(fd)` writes all string contents sequentially to the data section.

## Complete Example Usage

```c
// Phase 1: Code Generation
void printHello() {
    mov64_r_i(REG_RAX, 1);                           // syscall: write
    mov64_r_i(REG_RDI, 1);                           // fd: stdout
    create_variable_reference("string1", REG_RSI);    // address (to be patched)
    mov64_r_i(REG_RDX, 14);                          // length
    syscall_instruction();
}

int main() {
    // Generate code with unresolved references
    write_code();
    
    // Register string constants
    add_string_constant("string1", "Hello, World!\n");
    
    // Create ELF structure
    int fd = create_elf();
    
    // Phase 2: Resolution
    resolve_variable_addresses(phdrPublic.p_vaddr + get_current_code_size());
    
    // Phase 3: File Generation
    write_all_custom_code(fd);        // Write code with patched addresses
    write_string_constants_to_file(fd); // Write data section
    
    close(fd);
    return 0;
}
```
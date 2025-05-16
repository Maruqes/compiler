#include "bFunctions64.h"
#include "../functions.h"

/*
REX Prefix (64-bit mode):

O byte REX vai de 0x40 a 0x4F e é usado para aceder a registos extendidos e operandos de 64 bits.

Estrutura do byte REX: 0100WRXB (em binário)
  - Os 4 bits mais significativos são fixos: 0100
  - W (bit 3): Se 1, indica que o operando é de 64 bits (apenas para certas instruções)
  - R (bit 2): Extende o campo de registo (reg) do ModR/M (acesso a registos R8-R15)
  - X (bit 1): Extende o campo de índice (index) do byte SIB (acesso a registos R8-R15)
  - B (bit 0): Extende o campo de base (rm ou base) do ModR/M ou SIB (acesso a R8-R15)

Exemplo:
  0x4C → 01001100 → W=1, R=1, X=0, B=0
*/
void set_rex_prefix(char *opcode_bytes, uint8_t w, uint8_t r, uint8_t x, uint8_t b)
{
    // Constroi o byte REX com os bits W, R, X e B
    opcode_bytes[0] = 0x40 | (w << 3) | (r << 2) | (x << 1) | b;
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

void mov64_r_i(uint8_t reg_code, uint64_t value)
{
    // Allocate 10 bytes for REX + opcode + immediate (1 + 1 + 8)
    char *opcode_bytes = malloc(10);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix, handle high registers correctly
    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg_code >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xB8 + (reg_code & 0x7); // Opcode for 'mov r64, imm64'

    // Copy the 8-byte immediate value
    memcpy(&opcode_bytes[2], &value, sizeof(value));

    // Create the OpCode object
    OpCode new_opcode;
    new_opcode.size = 10;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes); // Free the just allocated memory to prevent leaks
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov64_r_r(uint8_t reg1, uint8_t reg2)
{
    // Allocate 3 bytes for REX + opcode
    char *opcode_bytes = malloc(3);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix, handle high registers correctly
    set_rex_prefix(opcode_bytes, 1, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x89;                                      // Opcode for 'mov r/m64, r64'
    opcode_bytes[2] = 0xC0 + (reg1 & 0x7) + ((reg2 & 0x7) << 3); // ModR/M byte

    // Create the OpCode object
    OpCode new_opcode;
    new_opcode.size = 3;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes); // Free the just allocated memory to prevent leaks
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function for the SYSCALL instruction (64-bit)
void syscall_instruction()
{
    char *opcode_bytes = malloc(2);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x0F; // First byte of syscall opcode
    opcode_bytes[1] = 0x05; // Second byte of syscall opcode

    OpCode new_opcode;
    new_opcode.size = 2;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move from memory to a 64-bit register
void mov64_r_m(uint8_t reg, uint8_t mem_reg)
{
    int usa_index = precisa_sib(MOD_1BYTE_DISP, mem_reg, 0);

    // Allocate 3 bytes for REX + opcode
    char *opcode_bytes = malloc(4 + usa_index);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    // Set REX prefix, handle high registers correctly
    set_rex_prefix(opcode_bytes, 1, (reg >= REG_R8) ? 1 : 0, 0, (mem_reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8B;
    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg, usa_index ? RM_SIB : mem_reg);

    if (usa_index)
    {
        set_sib(&opcode_bytes[3], 0, RM_SIB, mem_reg);
    }

    opcode_bytes[3 + usa_index] = 0x00; // Displacement byte (not used in this case)

    // Create the OpCode object
    OpCode new_opcode;
    new_opcode.size = 4 + usa_index;
    new_opcode.code = opcode_bytes;

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes); // Free the just allocated memory to prevent leaks
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move from memory with immediate offset to a 64-bit register
void mov64_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset)
{
    int usa_index = precisa_sib(MOD_4BYTE_DISP, reg_base, 0);

    char *opcode_bytes = malloc(7 + usa_index);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg_dest >= REG_R8) ? 1 : 0, 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8B;

    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg_dest, usa_index ? RM_SIB : reg_base);
    set_sib(&opcode_bytes[3], 0, RM_SIB, reg_base);
    memcpy(&opcode_bytes[3 + usa_index], &offset, sizeof(offset)); // Copy the 4-byte immediate offset

    OpCode new_opcode;
    new_opcode.size = 7 + usa_index;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move from memory with register offset to a 64-bit register
// mov    rax,QWORD PTR [reg_dest+reg_base*1]
void mov64_r_mr(uint8_t reg_dest, uint8_t reg_base, uint8_t reg_offset)
{
    // int usa_index = precisa_sib(MOD_1BYTE_DISP, reg_base, 1);
    int usa_index = 1; // usa index obrigatorio porque usa index

    char *opcode_bytes = malloc(5);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg_dest >= REG_R8) ? 1 : 0, (reg_offset >= REG_R8) ? 1 : 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x8B;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg_dest, RM_SIB);
    set_sib(&opcode_bytes[3], 0, reg_offset, reg_base);
    opcode_bytes[4] = 0x00; // Displacement byte (not used in this case)

    OpCode new_opcode;
    new_opcode.size = 5;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move immediate to memory
void mov64_m_i32(uint8_t reg1, uint32_t value)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(8 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC7;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, reg1);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg1);

    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte
    memcpy(&opcode_bytes[4 + usa_sib], &value, sizeof(value));

    OpCode new_opcode;
    new_opcode.size = 8 + usa_sib;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move register to memory
void mov64_m_r(uint8_t reg1, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg1, 0);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg2 >= REG_R8) ? 1 : 0, 0, (reg1 >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x89;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg2, reg1);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg1);
    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move immediate to memory with immediate offset
void mov64_mi_i(uint8_t reg, int32_t offset, int32_t value)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(11 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC7;

    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, 0, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset)); // Copy the 4-byte immediate offset
    memcpy(&opcode_bytes[7 + usa_sib], &value, sizeof(value));   // Copy the 4-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 11 + usa_sib;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move register to memory with immediate offset
void mov64_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2)
{
    int usa_sib = precisa_sib(MOD_4BYTE_DISP, reg, 0);

    char *opcode_bytes = malloc(7 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg2 >= REG_R8) ? 1 : 0, 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x89;

    set_modrm(&opcode_bytes[2], MOD_4BYTE_DISP, reg2, reg);
    set_sib(&opcode_bytes[3], SCALE_1, RM_SIB, reg);
    memcpy(&opcode_bytes[3 + usa_sib], &offset, sizeof(offset)); // Copy the 4-byte immediate offset

    OpCode new_opcode;
    new_opcode.size = 7 + usa_sib;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move immediate to memory with register offset
void mov64_mr_i(uint8_t reg, uint8_t reg2, int32_t value)
{
    int usa_sib = 1; // usa sib obrigatorio porque usa index

    char *opcode_bytes = malloc(9);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, 0, (reg2 >= REG_R8) ? 1 : 0, (reg >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0xC7;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, 0, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg2, reg);
    opcode_bytes[4] = 0x00;                          // Displacement byte
    memcpy(&opcode_bytes[5], &value, sizeof(value)); // Copy the 4-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 9;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

// Function to move register to memory with register offset
void mov64_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3)
{
    int usa_sib = precisa_sib(MOD_1BYTE_DISP, reg_base, 1);

    char *opcode_bytes = malloc(4 + usa_sib);
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_rex_prefix(opcode_bytes, 1, (reg3 >= REG_R8) ? 1 : 0, (reg2 >= REG_R8) ? 1 : 0, (reg_base >= REG_R8) ? 1 : 0);
    opcode_bytes[1] = 0x89;

    set_modrm(&opcode_bytes[2], MOD_1BYTE_DISP, reg3, RM_SIB);
    set_sib(&opcode_bytes[3], SCALE_1, reg2, reg_base);
    opcode_bytes[3 + usa_sib] = 0x00; // Displacement byte

    OpCode new_opcode;
    new_opcode.size = 4 + usa_sib;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array)
    {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

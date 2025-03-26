#include "bFunctions32.h"
#include "../functions.h"

/*
i-immediate
r-register
m-memory

functions:

mov32_16 r, i  :D
mov32_16 r, m  :D
mov32_16 r, mi :D
mov32_16 r, mr :D
mov32_16 r, r  :D

mov32_16 m, i  :D
mov32_16 m, r  :D

mov32_16 mi, i :D
mov32_16 mi, r :D

mov32_16 mr, i
mov32_16 mr, r
*/

void check_sixteen(uint8_t sixteen)
{
    if (sixteen != 0 && sixteen != 1)
    {
        perror("Sixteen must be 0 or 1");
        exit(EXIT_FAILURE);
    }
}

void set_sixteenByte(char *opcode_bytes, uint8_t sixteen)
{
    if (sixteen == 1)
    {
        opcode_bytes[0] = 0x66; // Prefix for 16-bit operand size
    }
}

// Function to move immediate value into a 32-bit register
void mov32_16_r_i(uint8_t reg_code, uint32_t value, uint8_t sixteen)
{
    if (reg_code == REG_ESP)
    {
        perror("Cannot move immediate value into ESP");
        exit(EXIT_FAILURE);
    }
    check_sixteen(sixteen);

    char *opcode_bytes = malloc(5 + sixteen); // 1-byte opcode + 4-byte immediate
    if (!opcode_bytes)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    set_sixteenByte(opcode_bytes, sixteen);

    opcode_bytes[0 + sixteen] = 0xB8 + reg_code; // Opcode for 'mov reg32, imm32'

    memcpy(&opcode_bytes[1 + sixteen], &value, sizeof(value)); // Copy the 4-byte immediate value

    OpCode new_opcode;
    new_opcode.size = 5 + sixteen; // Total instruction size
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

void mov32_16_r_m(uint8_t reg, uint8_t mem_reg, uint8_t sixteen)
{
    check_sixteen(sixteen);
    uint8_t *opcode_bytes;
    size_t size;

    if (mem_reg == REG_ESP)
    {
        size = 3 + sixteen;
        opcode_bytes = malloc(size);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0 + sixteen] = 0x8B;
        opcode_bytes[1 + sixteen] = 0x04 + (reg * 8);
        opcode_bytes[2 + sixteen] = 0x24;
    }
    else if (mem_reg == REG_EBP)
    {
        size = 3 + sixteen;
        opcode_bytes = malloc(size);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0 + sixteen] = 0x8B;
        opcode_bytes[1 + sixteen] = 0x45 + (reg * 8);
        opcode_bytes[2 + sixteen] = 0x00;
    }
    else // Caso normal, sem ESP/EBP
    {
        size = 2 + sixteen; // opcode + ModR/M
        opcode_bytes = malloc(size);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0 + sixteen] = 0x8B;                            // MOV r32, r/m32
        opcode_bytes[1 + sixteen] = (0 << 6) | (reg << 3) | mem_reg; // mod=00, reg, r/m=mem_reg
    }
    set_sixteenByte(opcode_bytes, sixteen);

    OpCode new_opcode;
    new_opcode.size = size;
    new_opcode.code = (char *)opcode_bytes;

    // Adiciona ao array global de opcodes
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void mov32_16_r_mi(uint8_t reg_dest, uint8_t reg_base, int32_t offset, uint8_t sixteen)
{
    check_sixteen(sixteen);
    OpCode new_opcode;
    char *opcode_bytes;
    if (reg_base == REG_ESP)
    {
        opcode_bytes = malloc(7 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0 + sixteen] = 0x8B;                  // Opcode for MOV reg32, r/m32
        opcode_bytes[1 + sixteen] = 0x84 + (reg_dest * 8); // Constructed ModR/M byte
        opcode_bytes[2 + sixteen] = 0x24;                  // SIB byte

        memcpy(&opcode_bytes[3 + sixteen], &offset, 4);

        new_opcode.size = 7 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    else if (reg_base == REG_EBP)
    {
        opcode_bytes = malloc(6 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0 + sixteen] = 0x8B;                  // Opcode for MOV reg32, r/m32
        opcode_bytes[1 + sixteen] = 0x85 + (reg_dest * 8); // Constructed ModR/M byte

        memcpy(&opcode_bytes[2 + sixteen], &offset, 4);

        new_opcode.size = 6 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    else
    {
        opcode_bytes = malloc(6 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }

        opcode_bytes[0 + sixteen] = 0x8B;                             // Opcode for MOV reg32, r/m32
        opcode_bytes[1 + sixteen] = 0x80 + reg_base + (reg_dest * 8); // Constructed ModR/M byte

        memcpy(&opcode_bytes[2 + sixteen], &offset, 4);

        new_opcode.size = 6 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    set_sixteenByte(opcode_bytes, sixteen);

    // Add the opcode to the array
    op_codes_array = realloc(op_codes_array,
                             (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}

void mov32_16_r_mr(uint8_t reg, uint8_t reg_base, uint8_t reg_offset, uint8_t sixteen)
{
    check_sixteen(sixteen);
    OpCode new_opcode;
    char *opcode_bytes;
    if (reg_base == REG_ESP)
    {
        opcode_bytes = malloc(3 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x8B;
        opcode_bytes[1 + sixteen] = 0x04 + (reg * 8);
        opcode_bytes[2 + sixteen] = 0x04 + (reg_offset * 8);

        new_opcode.size = 3 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    else if (reg_base == REG_EBP)
    {
        opcode_bytes = malloc(4 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x8B;
        opcode_bytes[1 + sixteen] = 0x44 + (reg * 8);
        opcode_bytes[2 + sixteen] = 0x05 + (reg_offset * 8);
        opcode_bytes[3 + sixteen] = 0x00;

        new_opcode.size = 4 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    else
    {
        opcode_bytes = malloc(3 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x8B;
        opcode_bytes[1 + sixteen] = 0x04 + (reg * 8);
        opcode_bytes[2 + sixteen] = (reg_base * 8) + reg_offset;

        new_opcode.size = 3 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    set_sixteenByte(opcode_bytes, sixteen);

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

void mov32_16_r_r(uint8_t reg1, uint8_t reg2, uint8_t sixteen)
{
    check_sixteen(sixteen);
    char *opcode_bytes = malloc(2 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0 + sixteen] = 0x89;
    opcode_bytes[1 + sixteen] = 0xC0 + reg1 + (reg2 * 8);
    set_sixteenByte(opcode_bytes, sixteen);

    OpCode new_opcode;
    new_opcode.size = 2 + sixteen;
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

void mov32_16_m_i(uint8_t reg1, uint32_t value, uint8_t sixteen)
{
    check_sixteen(sixteen);
    uint8_t *opcode_bytes;
    size_t size;

    if (reg1 == REG_ESP)
    {
        size = 7 + sixteen;
        opcode_bytes = malloc(size);

        opcode_bytes[0 + sixteen] = 0xC7;
        opcode_bytes[1 + sixteen] = 0x04;
        opcode_bytes[2 + sixteen] = 0x24;

        // Copiar os 4 bytes do immediate
        memcpy(&opcode_bytes[3 + sixteen], &value, 4);
    }
    else if (reg1 == REG_EBP)
    {
        size = 7 + sixteen;
        opcode_bytes = malloc(size);

        opcode_bytes[0 + sixteen] = 0xC7;
        opcode_bytes[1 + sixteen] = 0x45;
        opcode_bytes[2 + sixteen] = 0x00;

        memcpy(&opcode_bytes[3 + sixteen], &value, 4);
    }
    else
    {
        size = 6 + sixteen;
        opcode_bytes = malloc(size);

        opcode_bytes[0 + sixteen] = 0xC7;
        opcode_bytes[1 + sixteen] = (0 << 3) | reg1;
        memcpy(&opcode_bytes[2 + sixteen], &value, 4);
    }
    set_sixteenByte(opcode_bytes, sixteen);

    OpCode new_opcode;
    new_opcode.size = size;
    new_opcode.code = (char *)opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void mov32_16_m_r(uint8_t reg1, uint8_t reg2, uint8_t sixteen)
{
    check_sixteen(sixteen);
    OpCode new_opcode;
    uint8_t *opcode_bytes;

    if (reg1 == REG_ESP)
    {
        opcode_bytes = malloc(3 + sixteen);

        opcode_bytes[0 + sixteen] = 0x89;

        opcode_bytes[1 + sixteen] = 0x04 + (reg2 * 8);

        opcode_bytes[2 + sixteen] = 0x24;

        new_opcode.size = 3 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    else if (reg1 == REG_EBP)
    {
        opcode_bytes = malloc(3 + sixteen);

        opcode_bytes[0 + sixteen] = 0x89;

        opcode_bytes[1 + sixteen] = 0x45 + (reg2 * 8);

        opcode_bytes[2 + sixteen] = 0x00;

        new_opcode.size = 3 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    else
    {
        opcode_bytes = malloc(2 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x89;
        opcode_bytes[1 + sixteen] = 0x00 + reg1 + (reg2 * 8);

        new_opcode.size = 2 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    set_sixteenByte(opcode_bytes, sixteen);

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

void mov32_16_mi_i(uint8_t reg, int32_t offset, uint32_t value, uint8_t sixteen)
{

    check_sixteen(sixteen);
    OpCode new_opcode;

    char *opcode_bytes = malloc(10 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0 + sixteen] = 0xC7;
    opcode_bytes[1 + sixteen] = 0x80 + reg;
    memcpy(&opcode_bytes[2 + sixteen], &offset, 4);
    memcpy(&opcode_bytes[6 + sixteen], &value, 4);

    new_opcode.size = 10 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);

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

// works for all :D
void mov32_16_mi_r(uint8_t reg, uint32_t offset, uint8_t reg2, uint8_t sixteen)
{
    check_sixteen(sixteen);
    OpCode new_opcode;

    if (reg == REG_ESP)
    {
        char *opcode_bytes = malloc(7 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x89;
        opcode_bytes[1 + sixteen] = 0x84 + (reg2 * 8);
        opcode_bytes[2 + sixteen] = 0x24;
        memcpy(&opcode_bytes[3 + sixteen], &offset, 4);
        set_sixteenByte(opcode_bytes, sixteen);

        new_opcode.size = 7 + sixteen;
        new_opcode.code = opcode_bytes;
    }
    else
    {
        char *opcode_bytes = malloc(6 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x89;
        opcode_bytes[1 + sixteen] = 0x80 + reg + (reg2 * 8);
        memcpy(&opcode_bytes[2 + sixteen], &offset, 4);
        set_sixteenByte(opcode_bytes, sixteen);

        new_opcode.size = 6 + sixteen;
        new_opcode.code = opcode_bytes;
    }

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

void mov32_16_mr_i(uint8_t reg, uint8_t reg2, uint32_t value, uint8_t sixteen)
{
    check_sixteen(sixteen);
    char *opcode_bytes = malloc(7 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[0 + sixteen] = 0xC7;
    opcode_bytes[1 + sixteen] = 0x04;
    opcode_bytes[2 + sixteen] = (reg * 8) + reg2;

    memcpy(&opcode_bytes[3 + sixteen], &value, 4);

    OpCode new_opcode;
    new_opcode.size = 7 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);

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

void mov32_16_mr_r(uint8_t reg_base, uint8_t reg2, uint8_t reg3, uint8_t sixteen)
{
    check_sixteen(sixteen);
    OpCode new_opcode;

    if (reg_base == REG_ESP)
    {
        char *opcode_bytes = malloc(3 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x89;
        opcode_bytes[1 + sixteen] = 0x04 + (reg3 * 8);
        opcode_bytes[2 + sixteen] = 0x04 + (reg2 * 8);

        new_opcode.size = 3 + sixteen;
        new_opcode.code = opcode_bytes;
        set_sixteenByte(opcode_bytes, sixteen);
    }
    else if (reg_base == REG_EBP)
    {
        char *opcode_bytes = malloc(4 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x89;
        opcode_bytes[1 + sixteen] = 0x44 + (reg3 * 8);
        opcode_bytes[2 + sixteen] = 0x05 + (reg2 * 8);
        opcode_bytes[3 + sixteen] = 0x00;

        new_opcode.size = 4 + sixteen;
        new_opcode.code = opcode_bytes;
        set_sixteenByte(opcode_bytes, sixteen);
    }
    else
    {
        char *opcode_bytes = malloc(3 + sixteen);
        if (opcode_bytes == NULL)
        {
            perror("Failed to allocate memory for opcode_bytes");
            exit(EXIT_FAILURE);
        }
        opcode_bytes[0 + sixteen] = 0x89;
        opcode_bytes[1 + sixteen] = 0x04 + (reg3 * 8);
        opcode_bytes[2 + sixteen] = (reg_base * 8) + reg2;

        new_opcode.size = 3 + sixteen;
        new_opcode.code = opcode_bytes;
        set_sixteenByte(opcode_bytes, sixteen);
    }

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

void sete_r(uint8_t reg, uint8_t sixteen)
{
    check_sixteen(sixteen);
    // Instrução SETE r/m8 no modo registo:
    // 2 bytes de opcode (0x0F 0x94) + 1 byte ModR/M (mod = 11, reg = 0, r/m = reg)
    char *opcode_bytes = malloc(3 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    // Preenche a área reservada (após os primeiros 'sixteen' bytes)
    opcode_bytes[sixteen + 0] = 0x0F;
    opcode_bytes[sixteen + 1] = 0x94;
    // ModR/M: mod = 11 (0xC0) | r/m (valor do reg)
    opcode_bytes[sixteen + 2] = 0xC0 | reg;

    OpCode new_opcode;
    new_opcode.size = 3 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);

    // Adiciona o opcode ao array global
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}


void setne_r(uint8_t reg, uint8_t sixteen)
{
    check_sixteen(sixteen);
    // Instrução SETNE r/m8 no modo registo:
    // 2 bytes de opcode (0x0F 0x95) + 1 byte ModR/M (mod = 11, reg = 0, r/m = reg)
    char *opcode_bytes = malloc(3 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[sixteen + 0] = 0x0F;
    opcode_bytes[sixteen + 1] = 0x95;
    opcode_bytes[sixteen + 2] = 0xC0 | reg;
    
    OpCode new_opcode;
    new_opcode.size = 3 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void setl_r(uint8_t reg, uint8_t sixteen)
{
    check_sixteen(sixteen);
    // Instrução SETL r/m8: 2 bytes de opcode (0x0F 0x9C) + 1 byte ModR/M
    char *opcode_bytes = malloc(3 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[sixteen + 0] = 0x0F;
    opcode_bytes[sixteen + 1] = 0x9C;  // Opcode para SETL
    opcode_bytes[sixteen + 2] = 0xC0 | reg;  // ModR/M: mod 11, r/m = reg
    
    OpCode new_opcode;
    new_opcode.size = 3 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);
    
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void setg_r(uint8_t reg, uint8_t sixteen)
{
    check_sixteen(sixteen);
    // Instrução SETG r/m8: 2 bytes de opcode (0x0F 0x9F) + 1 byte ModR/M
    char *opcode_bytes = malloc(3 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    opcode_bytes[sixteen + 0] = 0x0F;
    opcode_bytes[sixteen + 1] = 0x9F;  // Opcode para SETG
    opcode_bytes[sixteen + 2] = 0xC0 | reg;  // ModR/M: mod 11, r/m = reg
    
    OpCode new_opcode;
    new_opcode.size = 3 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);
    
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

void shl_reg_imm(uint8_t reg, uint8_t imm, uint8_t sixteen)
{
    check_sixteen(sixteen);
    // Aloca espaço para 3 bytes de opcode + os bytes "sixteen"
    char *opcode_bytes = malloc(3 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    
    // Opcode: C1 /4 ib
    // Primeiro byte: C1
    opcode_bytes[sixteen + 0] = 0xC1;
    // Segundo byte: ModR/M – mod 11 (0xC0) | (/4 = 4<<3 = 0x20) | reg (valor do registo)
    opcode_bytes[sixteen + 1] = 0xC0 | (4 << 3) | reg; // equivale a 0xE0 + reg
    // Terceiro byte: valor imediato
    opcode_bytes[sixteen + 2] = imm;
    
    OpCode new_opcode;
    new_opcode.size = 3 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);
    
    // Adiciona o opcode ao array global
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}


void shr_reg_imm(uint8_t reg, uint8_t imm, uint8_t sixteen)
{
    check_sixteen(sixteen);
    // Aloca espaço para 3 bytes de opcode + os bytes "sixteen"
    char *opcode_bytes = malloc(3 + sixteen);
    if (opcode_bytes == NULL)
    {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }
    
    // Opcode: C1 /5 ib
    // Primeiro byte: C1
    opcode_bytes[sixteen + 0] = 0xC1;
    // Segundo byte: ModR/M – mod 11 (0xC0) | (/5 = 5<<3 = 0x28) | reg (valor do registo)
    opcode_bytes[sixteen + 1] = 0xC0 | (5 << 3) | reg; // equivale a 0xE8 + reg
    // Terceiro byte: valor imediato
    opcode_bytes[sixteen + 2] = imm;
    
    OpCode new_opcode;
    new_opcode.size = 3 + sixteen;
    new_opcode.code = opcode_bytes;
    set_sixteenByte(opcode_bytes, sixteen);
    
    // Adiciona o opcode ao array global
    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (op_codes_array == NULL)
    {
        perror("Failed to reallocate memory for op_codes_array");
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size] = new_opcode;
    op_codes_array_size++;
}

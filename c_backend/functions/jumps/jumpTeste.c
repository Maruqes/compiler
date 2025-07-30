#include "jumps.h"
#include "../functions.h"
#include "../bFunctions64/bFunctions64.h"
#include <stdint.h>
#include <limits.h>
#include <stdio.h>

// Test arrays for different register combinations
static const uint8_t all_regs[] = {
    REG_RAX, REG_RCX, REG_RDX, REG_RBX,
    REG_RBP, REG_RSI, REG_RDI, // Skip RSP as it's not allowed for jump_reg
    REG_R8, REG_R9, REG_R10, REG_R11,
    REG_R12, REG_R13, REG_R14, REG_R15};

// Test offsets for short jumps (8-bit)
static const uint8_t short_offsets[] = {0, 1, 10, 50, 127, 128, 200, 255};

// Test offsets for near jumps (32-bit)
static const uint32_t near_offsets[] = {
    0U, 1U, 100U, 1000U,
    0x7FFFFFFFU, 0x80000000U,
    0xFFFFFFFEU, UINT32_MAX};

// All conditional jump opcodes to test
static const uint8_t jcc_conditions[] = {
    JE_OPCODE,  // Jump if Equal
    JNE_OPCODE, // Jump if Not Equal
    JG_OPCODE,  // Jump if Greater
    JL_OPCODE,  // Jump if Less
    JGE_OPCODE, // Jump if Greater or Equal
    JLE_OPCODE, // Jump if Less or Equal
    JA_OPCODE,  // Jump if Above (unsigned)
    JB_OPCODE,  // Jump if Below (unsigned)
    JAE_OPCODE, // Jump if Above or Equal (unsigned)
    JBE_OPCODE  // Jump if Below or Equal (unsigned)
};

// Test function for jump_short
void test_jump_short(void)
{
    printf("Testing jump_short function...\n");

    for (unsigned i = 0; i < sizeof(short_offsets) / sizeof(*short_offsets); ++i)
    {
        printf("  Testing jump_short with offset: %u\n", short_offsets[i]);
        jump_short(short_offsets[i]);
        nop(); // Add nop for separation
    }

    printf("jump_short tests completed.\n\n");
}

// Test function for jump_near
void test_jump_near(void)
{
    printf("Testing jump_near function...\n");

    for (unsigned i = 0; i < sizeof(near_offsets) / sizeof(*near_offsets); ++i)
    {
        printf("  Testing jump_near with offset: %u\n", near_offsets[i]);
        jump_near(near_offsets[i]);
        nop(); // Add nop for separation
    }

    printf("jump_near tests completed.\n\n");
}

// Test function for jump_reg
void test_jump_reg(void)
{
    printf("Testing jump_reg function...\n");

    for (unsigned i = 0; i < sizeof(all_regs) / sizeof(*all_regs); ++i)
    {
        printf("  Testing jump_reg with register: 0x%02X\n", all_regs[i]);
        jump_reg(all_regs[i]);
        nop(); // Add nop for separation
    }

    printf("jump_reg tests completed.\n\n");
}

// Test function for jmp with labels
void test_jmp_labels(void)
{
    printf("Testing jmp function with labels...\n");

    // Create some test labels
    create_label("test_label_1");
    nop();
    nop();

    create_label("test_label_2");
    nop();
    nop();

    create_label("loop_start");
    nop();

    // Test jumps to these labels
    printf("  Testing jmp to test_label_1\n");
    jmp("test_label_1");

    printf("  Testing jmp to test_label_2\n");
    jmp("test_label_2");

    printf("  Testing jmp to loop_start\n");
    jmp("loop_start");

    create_label("end_label");

    printf("jmp with labels tests completed.\n\n");
}

// Test function for jcc (conditional jumps)
void test_jcc_conditions(void)
{
    printf("Testing jcc function with all conditions...\n");

    // Create labels for conditional jumps
    create_label("je_target");
    nop();
    create_label("jne_target");
    nop();
    create_label("jg_target");
    nop();
    create_label("jl_target");
    nop();
    create_label("jge_target");
    nop();
    create_label("jle_target");
    nop();
    create_label("ja_target");
    nop();
    create_label("jb_target");
    nop();
    create_label("jae_target");
    nop();
    create_label("jbe_target");
    nop();

    const char *condition_names[] = {
        "JE (Jump if Equal)",
        "JNE (Jump if Not Equal)",
        "JG (Jump if Greater)",
        "JL (Jump if Less)",
        "JGE (Jump if Greater or Equal)",
        "JLE (Jump if Less or Equal)",
        "JA (Jump if Above)",
        "JB (Jump if Below)",
        "JAE (Jump if Above or Equal)",
        "JBE (Jump if Below or Equal)"};

    const char *label_names[] = {
        "je_target", "jne_target", "jg_target", "jl_target", "jge_target",
        "jle_target", "ja_target", "jb_target", "jae_target", "jbe_target"};

    for (unsigned i = 0; i < sizeof(jcc_conditions) / sizeof(*jcc_conditions); ++i)
    {
        printf("  Testing %s (opcode: 0x%02X)\n", condition_names[i], jcc_conditions[i]);
        jcc((char *)label_names[i], jcc_conditions[i]);
        nop(); // Add nop for separation
    }

    printf("jcc conditional jumps tests completed.\n\n");
}

void funcao_teste_all_jumps(void)
{
    test_jump_short();
    test_jump_near();
    test_jump_reg();

    // Test label-based jumps
    test_jmp_labels();

    // Test conditional jumps
    test_jcc_conditions();
}

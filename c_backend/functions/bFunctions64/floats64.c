#include "bFunctions64.h"
#include "../functions.h"

void movq_x_r(uint8_t xmm, uint8_t r64)
{
    // movq xmm, r64: 66 0F 6E /r   (SSE2 form, NOT MMX)
    uint8_t *opcode_bytes = malloc(5);
    if (!opcode_bytes) {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // SSE2 mandatory prefix

    // REX: W=1 (64-bit), R=high bit of xmm, B=high bit of r64
    set_rex_prefix(&opcode_bytes[1], 1, (xmm >= 8), 0, (r64 >= 8));

    opcode_bytes[2] = 0x0F;
    opcode_bytes[3] = 0x6E;

    // ModR/M: reg = xmm, rm = r64
    set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm & 0x7, r64 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 5;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array) {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}


void movq_r_x(uint8_t r64, uint8_t xmm)
{
    // movq r64, xmm: 66 0F 7E /r   (SSE2 form, NOT MMX)
    uint8_t *opcode_bytes = malloc(5);
    if (!opcode_bytes) {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0x66; // SSE2 mandatory prefix

    // REX: W=1 (64-bit), R=high bit of r64, B=high bit of xmm
    set_rex_prefix(&opcode_bytes[1], 1, (r64 >= 8), 0, (xmm >= 8));

    opcode_bytes[2] = 0x0F;
    opcode_bytes[3] = 0x7E;

    // ModR/M: reg = r64, rm = xmm
    set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, r64 & 0x7, xmm & 0x7);

    OpCode new_opcode;
    new_opcode.size = 5;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array) {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }
    op_codes_array[op_codes_array_size++] = new_opcode;
}


void addsd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// addsd xmm1, xmm2: opcode F2 0F 58 /r
	// REX + prefix + 0F + opcode + ModR/M = 5 bytes
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	// REX: W=0, R=high bit of xmm1, X=0, B=high bit of xmm2
	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);

	opcode_bytes[1] = 0xF2; // Prefix for scalar double-precision
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x58;

	// ModR/M: mod=11 (reg-direct), reg=xmm1 dest, rm=xmm2 source
	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void subsd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// subsd xmm1, xmm2: opcode F2 0F 5C /r
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	// REX: W=0, R=high bit of xmm1, X=0, B=high bit of xmm2
	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);

	opcode_bytes[1] = 0xF2;
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x5C;

	// ModR/M: mod=11 (reg-direct), reg=xmm1 dest, rm=xmm2 source
	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void mulsd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// mulsd xmm1, xmm2: opcode F2 0F 59 /r
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	// REX: W=0, R=high bit of xmm1, X=0, B=high bit of xmm2
	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);

	opcode_bytes[1] = 0xF2;
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x59;

	// ModR/M: mod=11 (reg-direct), reg=xmm1 dest, rm=xmm2 source
	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void divsd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// divsd xmm1, xmm2: opcode F2 0F 5E /r
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	// REX: W=0, R=high bit of xmm1, X=0, B=high bit of xmm2
	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);

	opcode_bytes[1] = 0xF2;
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x5E;

	// ModR/M: mod=11 (reg-direct), reg=xmm1 dest, rm=xmm2 source
	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void ucomisd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// ucomisd xmm1, xmm2: opcode 66 0F 2E /r (unordered compare)
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);
	opcode_bytes[1] = 0x66; // operand-size prefix for SSE2 scalar
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x2E;

	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void comisd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// comisd xmm1, xmm2: opcode 66 0F 2F /r (compare)
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);
	opcode_bytes[1] = 0x66;
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x2F;

	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void xorpd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// xorpd xmm1, xmm2: opcode 66 0F EF /r (useful for negation by XOR with mask)
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);
	opcode_bytes[1] = 0x66;
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0xEF;

	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void andpd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// andpd xmm1, xmm2: opcode 66 0F 54 /r (useful for absolute value via mask)
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);
	opcode_bytes[1] = 0x66;
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x54;

	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void sqrtsd_x_x(uint8_t xmm1, uint8_t xmm2)
{
	// sqrtsd xmm1, xmm2: opcode F2 0F 51 /r
	uint8_t *opcode_bytes = malloc(5);
	if (!opcode_bytes)
	{
		perror("Failed to allocate memory for opcode_bytes");
		exit(EXIT_FAILURE);
	}

	set_rex_prefix(opcode_bytes, 0, (xmm1 >= 8) ? 1 : 0, 0, (xmm2 >= 8) ? 1 : 0);
	opcode_bytes[1] = 0xF2;
	opcode_bytes[2] = 0x0F;
	opcode_bytes[3] = 0x51;

	set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm1 & 0x7, xmm2 & 0x7);

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

void cvtsi2sd_x_r(uint8_t xmm, uint8_t r64) {
    // cvtsi2sd xmm, r64: F2 0F 2A /r
    // Needs REX.W=1 for 64-bit
    uint8_t *opcode_bytes = malloc(5);
    if (!opcode_bytes) {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF2; // mandatory prefix
    set_rex_prefix(&opcode_bytes[1], 1, (xmm >= 8), 0, (r64 >= 8)); // REX.W=1
    opcode_bytes[2] = 0x0F;
    opcode_bytes[3] = 0x2A; // opcode
    set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, xmm & 0x7, r64 & 0x7);

    OpCode new_opcode;
    new_opcode.size = 5;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array) {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

void cvttsd2si_r_x(uint8_t r64, uint8_t xmm) {
    // cvttsd2si r64, xmm: F2 0F 2C /r
    uint8_t *opcode_bytes = malloc(5);
    if (!opcode_bytes) {
        perror("Failed to allocate memory for opcode_bytes");
        exit(EXIT_FAILURE);
    }

    opcode_bytes[0] = 0xF2; // mandatory prefix
    set_rex_prefix(&opcode_bytes[1], 1, (r64 >= 8), 0, (xmm >= 8)); // REX.W=1
    opcode_bytes[2] = 0x0F;
    opcode_bytes[3] = 0x2C; // opcode
    set_modrm(&opcode_bytes[4], MOD_REG_DIRECT, r64 & 0x7, xmm & 0x7);

    OpCode new_opcode;
    new_opcode.size = 5;
    new_opcode.code = opcode_bytes;

    op_codes_array = realloc(op_codes_array, (op_codes_array_size + 1) * sizeof(OpCode));
    if (!op_codes_array) {
        perror("Failed to reallocate memory for op_codes_array");
        free(opcode_bytes);
        exit(EXIT_FAILURE);
    }

    op_codes_array[op_codes_array_size++] = new_opcode;
}

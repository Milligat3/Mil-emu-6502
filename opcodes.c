// opcodes.c

#define STACK_ADDR 0x100

typedef void (*op_handler)(void);

void push_st(uint8_t op)
{
	memory[STACK_ADDR + cpu.SP] = op;
	cpu.SP--;
}

uint8_t pop_st(void)
{
	cpu.SP++;
	return memory[STACK_ADDR + cpu.SP];
}

uint16_t addr_abs(void)
{
	uint8_t lower = memory[cpu.PC++];
	uint8_t higher = memory[cpu.PC++];
	return lower | (higher << 8);
}

uint16_t addr_indexed_indirect(void)
{
	uint16_t pt_base = (memory[cpu.PC++] + cpu.X) & 0xFF;
	uint8_t lower = memory[pt_base];
	uint8_t higher = memory[(pt_base + 1) & 0xFF];
	return lower | (higher << 8);
}

uint16_t addr_indirect_indexed(void)
{
	uint8_t pt_base = memory[cpu.PC++];
	uint8_t lower = memory[pt_base];
	uint8_t higher = memory[(pt_base + 1) & 0xFF];
	uint16_t base = lower | (higher << 8);
	return base + cpu.Y;
}

// LDA FAMILY

void lda_exec(uint16_t addr)
{
    cpu.A = memory[addr];
    set_NZ_flags(cpu.A);
}

void op_A9(void) // LDA imm
{
	uint16_t addr = cpu.PC++;
	lda_exec(addr);
}

void op_A5(void) // LDA zpg
{
	uint16_t addr = memory[cpu.PC++];
	lda_exec(addr);
}

void op_B5(void) // LDA zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	lda_exec(addr);
}

void op_AD(void) // LDA abs
{

	uint16_t addr = addr_abs();
	lda_exec(addr);
}

void op_BD(void) // LDA abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	lda_exec(addr);
}

void op_B9(void) // LDA abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	lda_exec(addr);
}

void op_A1(void) // LDA (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	lda_exec(addr);
}

void op_B1(void) // LDA (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	lda_exec(addr);
}

// LDX FAMILY

void ldx_exec(uint16_t addr)
{
	cpu.X = memory[addr];
	set_NZ_flags(cpu.X);
}

void op_A2(void) // LDX imm
{
	uint16_t addr = cpu.PC++;
	ldx_exec(addr);
}

void op_A6(void) // LDX zpg
{
	uint16_t addr = memory[cpu.PC++];
	ldx_exec(addr);
}

void op_B6(void) // LDX zpg, Y
{
	uint16_t addr = (memory[cpu.PC++] + cpu.Y) & 0xFF;
	ldx_exec(addr);
}

void op_AE(void) // LDX abs
{
	uint16_t addr = addr_abs();
	ldx_exec(addr);
}

void op_BE(void) // LDX abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	ldx_exec(addr);
}

// LDY FAMILY

void ldy_exec(uint16_t addr)
{
	cpu.Y = memory[addr];
	set_NZ_flags(cpu.Y);
}

void op_A0(void) // LDY imm
{
	uint16_t addr = cpu.PC++;
	ldy_exec(addr);
}

void op_A4(void) // LDY zpg
{
	uint16_t addr = memory[cpu.PC++];
	ldy_exec(addr);
}

void op_B4(void) // LDY zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	ldy_exec(addr);
}

void op_AC(void) // LDY abs
{
	uint16_t addr = addr_abs();
	ldy_exec(addr);
}

void op_BC(void) // LDY abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	ldy_exec(addr);
}

// JMP, Branching
void op_4C(void) // JMP abs
{
	uint16_t addr_jmp_abs = addr_abs();
	cpu.PC = addr_jmp_abs;
}

void op_6C(void) // JMP indirect
{
	uint16_t ptr = addr_abs();
	uint8_t lower = memory[ptr];
	uint8_t higher = 0;
	if((lower & 0xFF) == 0xFF)
	{
		higher = memory[ptr & 0xFF];
	}
	else
	{
		higher = memory[ptr + 1];
	}
	uint16_t addr = lower | (higher << 8);
	cpu.PC = addr;
}

// Bxx instructions

void branch(int condition)
{
	if(condition)
	{
		int8_t offset = (int8_t)(memory[cpu.PC++]);
		// uint16_t old_pc = cpu.PC;
		cpu.PC += offset;
	}
	else
	{
		cpu.PC++;
	}
}

void op_B0(void) // BCS rel
{
	branch(cpu.C == 1);
}

void op_90(void) // BCC rel
{
	branch(cpu.C == 0);
}

void op_F0(void) // BEQ rel
{
	branch(cpu.Z == 1);
}

void op_D0(void) // BNE rel
{
	branch(cpu.Z == 0);
}

void op_30(void) // BMI rel
{
	branch(cpu.N == 1);
}

void op_10(void) // BPL rel
{
	branch(cpu.N == 0);
}

void op_70(void) // BVS rel
{
	branch(cpu.V == 1);
}

void op_50(void) // BVC rel
{
	branch(cpu.V == 0);
}

// BitShifts!
// ASL family

void asl_exec(uint16_t addr)
{
	uint8_t val = memory[addr];
	uint16_t res = val << 1;
	set_NZ_flags(res & 0xFF);
	cpu.C = res >> 8;
	memory[addr] = (uint8_t) (res & 0xFF);
}

void op_0A(void) // ASL A
{
	uint16_t result = cpu.A << 1;
	cpu.C = result >> 8;
	cpu.A = (uint8_t) (result & 0xFF);
	set_NZ_flags(cpu.A);
}

void op_06(void) // ASL zpg
{
	uint16_t addr = memory[cpu.PC++];
	asl_exec(addr);
}

void op_16(void) // ASL zpg, X;
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	asl_exec(addr);
}

void op_0E(void) // ASL abs
{
	uint16_t addr = addr_abs();
	asl_exec(addr);
}

void op_1E(void) // ASL abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	asl_exec(addr);
}

// LSR family

void lsr_exec(uint16_t addr)
{
	uint8_t value = memory[addr];
	cpu.C = value & 0x1;
	value = (value >> 1) & 0x7F;
	memory[addr] = value;
	set_NZ_flags(value);
}

void op_4A(void) // LSR A
{
	cpu.C = cpu.A & 0x1;
	cpu.A = (cpu.A >> 1) & 0x7F;
	set_NZ_flags(cpu.A);
}

void op_46(void) // LSR zpg
{
	uint16_t addr = memory[cpu.PC++];
	lsr_exec(addr);
}

void op_56(void) // LSR zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	lsr_exec(addr);
}

void op_4E(void) // LSR abs
{
	uint16_t addr = addr_abs();
	lsr_exec(addr);
}

void op_5E(void) // LSR abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	lsr_exec(addr);
}

// JSR/RTS
void op_20(void) // JSR
{
	uint8_t low = memory[cpu.PC++];
	uint8_t hi = memory[cpu.PC++];
	uint16_t return_addr = cpu.PC - 1;
	push_st((return_addr >> 8) & 0xFF);
	push_st(return_addr & 0xFF);

	uint16_t target_addr = low | (hi << 8);
	cpu.PC = target_addr;
}


void op_60(void) // RTS
{
	uint8_t pcl = pop_st();
	uint8_t pch = pop_st();
	cpu.PC = pcl | (pch << 8);
	cpu.PC++;
}

// STA family

void sta_exec(uint16_t addr)
{
	memory[addr] = cpu.A;
}

void op_85(void) // STA zpg
{
	uint16_t addr = memory[cpu.PC++];
	sta_exec(addr);
}

void op_95(void) // STA zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	sta_exec(addr);
}

void op_8D(void) // STA abs
{
	uint16_t addr = addr_abs();
	sta_exec(addr);
}

void op_9D(void) // STA abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	sta_exec(addr);
}

void op_99(void) // STA abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	sta_exec(addr);
}

void op_81(void) // STA (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	sta_exec(addr);
}

void op_91(void) // STA (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	sta_exec(addr);
}

// STX family

void stx_exec(uint16_t addr)
{
	memory[addr] = cpu.X;
}

void op_86(void) // STX zpg
{
	uint16_t addr = memory[cpu.PC++];
	stx_exec(addr);
}

void op_96(void) // STX zpg, Y
{
	uint16_t addr = (memory[cpu.PC++] + cpu.Y) & 0xFF;
	stx_exec(addr);
}

void op_8E(void) // STX abs
{
	uint16_t addr = addr_abs();
	stx_exec(addr);
}

// STY family

void sty_exec(uint16_t addr)
{
	memory[addr] = cpu.Y;
}

void op_84(void) // STY zpg
{
	uint16_t addr = memory[cpu.PC++];
	sty_exec(addr);
}

void op_94(void) // STY zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	sty_exec(addr);
}

void op_8C(void) // STY abs
{
	uint16_t addr = addr_abs();
	sty_exec(addr);
}

// INC, DEC

void op_C8(void) // INY
{
	cpu.Y++;
	set_NZ_flags(cpu.Y);
}

void op_E8(void) // INX
{
	cpu.X++;
	set_NZ_flags(cpu.X);
}

void op_88(void) // DEY
{
	cpu.Y--;
	set_NZ_flags(cpu.Y);
}

void op_CA(void) // DEX
{
	cpu.X--;
	set_NZ_flags(cpu.X);
}

// DEC family
void dec_exec(uint16_t addr)
{
	memory[addr]--;
	set_NZ_flags(memory[addr]);
}

void op_C6(void) // DEC zpg
{
	uint16_t addr = memory[cpu.PC++];
	dec_exec(addr);
}

void op_D6(void) // DEC zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	dec_exec(addr);
}

void op_CE(void) // DEC abs
{
	uint16_t addr = addr_abs();
	dec_exec(addr);
}

void op_DE(void) // DEC abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	dec_exec(addr);
}

// INC family

void inc_exec(uint16_t addr)
{
	memory[addr]++;
	set_NZ_flags(memory[addr]);
}

void op_E6(void) // INC zpg
{
	uint16_t addr = memory[cpu.PC++];
	inc_exec(addr);
}

void op_F6(void) // INC zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	inc_exec(addr);
}

void op_EE(void) // INC abs
{
	uint16_t addr = addr_abs();
	inc_exec(addr);
}

void op_FE(void) // INC abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	inc_exec(addr);
}

void op_00(void) // BRK
{
	push_st((uint8_t)((cpu.PC+1) >> 8));
	push_st((uint8_t)((cpu.PC+1) & 0xFF));
	uint8_t status = cpu.flags | 0x30;
	push_st(status);
	printf("BRK encountered at 0x%x\n", cpu.PC - 1);
	cpu.I = 1;
	cpu.PC = memory[0xFFFE] | (memory[0xFFFF] << 8);
}

void op_40(void) // RTI
{
	cpu.flags = pop_st();
	uint8_t pc_lo = pop_st();
	uint8_t pc_hi = pop_st();
	cpu.PC = pc_lo | (pc_hi << 8);
}

// Clear/Set flags

void op_18(void) // CLC
{
	cpu.C = 0;
}

void op_D8(void) // CLD
{
	cpu.D = 0;
}

void op_58(void) // CLI
{
	cpu.I = 0;
}

void op_B8(void) // CLV
{
	cpu.V = 0;
}

void op_38(void) // SEC
{
	cpu.C = 1;
}

void op_F8(void) // SED
{
	cpu.D = 1;
}

void op_78(void) // SEI
{
	cpu.I = 1;
}

// ADC family

void adc_exec(uint16_t addr)
{
	if(cpu.D == 0)
	{
		uint16_t result = cpu.A + memory[addr] + cpu.C;
		cpu.V = (~(cpu.A ^ memory[addr]) & (cpu.A ^ result) & 0x80) >> 7; // If significant bits of 1-st and 2-nd operands are same (XOR gives us 0 and ~ gives os 1 baack) and 1-st operand significant bit is not the same as result's then we have overflow
		set_NZ_flags((uint8_t)(result & 0xFF));
		cpu.C = (result & 0x100) >> 8;
		cpu.A = result & 0xFF;
	}
	else
	{
		uint8_t res_l = (cpu.A & 0xF) + (memory[addr] & 0xF) + cpu.C;
		if (res_l > 9) res_l += 6;
		uint8_t carry_l = res_l >> 4;
		res_l &= 0x0F;
		uint8_t res_h = (cpu.A >> 4) + (memory[addr] >> 4) + carry_l;
		uint8_t carry_h = 0;
		if(res_h > 9)
		{
			res_h += 6;
			carry_h = 1;
		}
		res_h &= 0x0F;
		uint16_t bin_res = cpu.A + memory[addr] + cpu.C;
		cpu.A = res_l | (res_h << 4);
		cpu.Z = (bin_res == 0);
		cpu.N = (cpu.A & 0x80) != 0;
		cpu.V = ((~(cpu.A ^ memory[addr])) & (cpu.A ^ bin_res) & 0x80) ? 1 : 0; // For V flag we check binary result, not BCD
		cpu.C = carry_h || (res_h >= 10);
	}
}

void op_69(void) // ADC imm
{
	uint16_t addr = cpu.PC++;
	adc_exec(addr);
}

void op_65(void) // ADC zpg
{
	uint16_t addr = memory[cpu.PC++];
	adc_exec(addr);
}

void op_75(void) // ADC zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	adc_exec(addr);
}

void op_6D(void) // ADC abs
{
	uint16_t addr = addr_abs();
	adc_exec(addr);
}

void op_7D(void) // ADC abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	adc_exec(addr);
}

void op_79(void) // ADC abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	adc_exec(addr);
}

void op_61(void) // ADC (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	adc_exec(addr);
}

void op_71(void) // ADC (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	adc_exec(addr);
}

// SBC family
void sbc_exec(uint16_t addr)
{
	if(cpu.D == 0)
	{
		uint16_t result = cpu.A - memory[addr] - (1 - cpu.C);
		cpu.V = ((cpu.A ^ memory[addr]) & (cpu.A ^ result) & 0x80) >> 7; // If significant bits of 1-st and 2-nd operands are diff (XOR gives us 1) and 1-st operand significant bit is not the same as result's then we have overflow
		set_NZ_flags((uint8_t)(result & 0xFF));
		cpu.C = (result <= 0xFF);
		cpu.A = result & 0xFF;
	}
	else
	{
		uint16_t normal_res = cpu.A - memory[addr] - (1 - cpu.C);

		uint8_t res_l = (cpu.A & 0xF) - (memory[addr] & 0xF) - (1 - cpu.C);
		uint8_t borrow_l = res_l & 0x10;
		if(borrow_l) res_l -= 6;
		res_l &= 0x0F;

		uint8_t res_h = (cpu.A >> 4) - (memory[addr] >> 4) - (borrow_l != 0);
		uint8_t borrow_h = res_h & 0x10;
		if (borrow_h) res_h -= 6;
		res_h &= 0x0F;
		cpu.A = (res_h << 4) | res_l;

		cpu.C = !borrow_h;
		cpu.Z = (cpu.A == 0);
		cpu.V = ((normal_res ^ cpu.A) && (cpu.A ^ memory[addr]) & 0x80) ? 1: 0;
		cpu.N = (normal_res & 0x80) != 0;
	}
}

void op_E9(void) // SBC imm
{
	uint16_t addr = cpu.PC++;
	sbc_exec(addr);
}

void op_E5(void) // SBC zpg
{
	uint16_t addr = memory[cpu.PC++];
	sbc_exec(addr);
}

void op_F5(void) // SBC zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	sbc_exec(addr);
}

void op_ED(void) // SBC abs
{
	uint16_t addr = addr_abs();
	sbc_exec(addr);
}

void op_FD(void) // SBC abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	sbc_exec(addr);
}

void op_F9(void) // SBC abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	sbc_exec(addr);
}

void op_E1(void) // SBC (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	sbc_exec(addr);
}

void op_F1(void) // SBC (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	sbc_exec(addr);
}

// CMP family

void cmp_exec(uint16_t addr)
{
	uint16_t result = cpu.A - memory[addr];

	set_NZ_flags((uint8_t)(result & 0xFF));
	cpu.C = (result <= 0xFF);
}

void op_C9(void) // CMP imm
{
	uint16_t addr = cpu.PC++;
	cmp_exec(addr);
}

void op_C5(void) // CMP zpg
{
	uint16_t addr = memory[cpu.PC++];
	cmp_exec(addr);
}

void op_D5(void) // CMP zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	cmp_exec(addr);
}

void op_CD(void) // CMP abs
{
	uint16_t addr = addr_abs();
	cmp_exec(addr);
}

void op_DD(void) // CMP abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	cmp_exec(addr);
}

void op_D9(void) // CMP abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	cmp_exec(addr);
}

void op_C1(void) // CMP (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	cmp_exec(addr);
}

void op_D1(void) // CMP (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	cmp_exec(addr);
}

// CPX family
void cpx_exec(uint16_t addr)
{
	uint16_t result = cpu.X - memory[addr];

	set_NZ_flags((uint8_t)(result & 0xFF));
	cpu.C = (result <= 0xFF);
}

void op_E0(void) // CPX imm
{
	uint16_t addr = cpu.PC++;
	cpx_exec(addr);
}

void op_E4(void) // CPX zpg
{
	uint16_t addr = memory[cpu.PC++];
	cpx_exec(addr);
}

void op_EC(void) // CPX abs
{
	uint16_t addr = addr_abs();
	cpx_exec(addr);
}
// CPY family

void cpy_exec(uint16_t addr)
{
	uint16_t result = cpu.Y - memory[addr];

	set_NZ_flags((uint8_t)(result & 0xFF));
	cpu.C = (result <= 0xFF);
}

void op_C0(void) // CPY imm
{
	uint16_t addr = cpu.PC++;
	cpy_exec(addr);
}

void op_C4(void) // CPY zpg
{
	uint16_t addr = memory[cpu.PC++];
	cpy_exec(addr);
}

void op_CC(void) // CPY abs
{
	uint16_t addr = addr_abs();
	cpy_exec(addr);
}

// PH-PL family
void op_48(void) // PHA
{
	push_st(cpu.A);
}

void op_68(void) // PLA
{
	cpu.A = pop_st();
	set_NZ_flags(cpu.A);
}

void op_08(void) // PHP
{
	uint8_t status = cpu.flags | 0x30;
	push_st(status);
}

void op_28(void) // PLP
{
	cpu.flags = pop_st();
}

// Txx family

void op_AA(void) // TAX
{
	cpu.X = cpu.A;
	set_NZ_flags(cpu.X);
}

void op_A8(void) // TAY
{
	cpu.Y = cpu.A;
	set_NZ_flags(cpu.Y);
}

void op_BA(void) // TSX
{
	cpu.X = cpu.SP;
	set_NZ_flags(cpu.X);
}

void op_8A(void) // TXA
{
	cpu.A = cpu.X;
	set_NZ_flags(cpu.A);
}

void op_9A(void) // TXS
{
	cpu.SP = cpu.X;
}

void op_98(void) // TYA
{
	cpu.A = cpu.Y;
	set_NZ_flags(cpu.A);
}

// BIT family(small, but honest!)

void bit_exec(uint16_t addr) // This is for execution, so I don't have to double code for every adressing mode
{
	uint8_t result = cpu.A & memory[addr];
	if(result == 0) cpu.Z = 1;
	else cpu.Z = 0;
	cpu.N = (memory[addr] & 0x80) >> 7;
	cpu.V = (memory[addr] & 0x40) >> 6;
}

void op_24(void) // BIT zpg
{
	uint16_t addr = memory[cpu.PC++];
	bit_exec(addr);
}

void op_2C(void) // BIT abs
{
	uint16_t addr = addr_abs();
	bit_exec(addr);
}

void op_EA(void) // NOP
{
	// waste cycles so.. NIP! For now - skip. Later - will add some quirky magic - counting cycles
	return;
}

// Logic opcodes AND, ORA, EOR...

// AND family

void and_exec(uint16_t addr)
{
	cpu.A = cpu.A & memory[addr];
	set_NZ_flags(cpu.A);
}

void op_29(void) // AND imm
{
	uint16_t addr = cpu.PC++;
	and_exec(addr);
}

void op_25(void) // AND zpg
{
	uint16_t addr = memory[cpu.PC++];
	and_exec(addr);
}

void op_35(void) // AND zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	and_exec(addr);
}

void op_2D(void) // AND abs
{
	uint16_t addr = addr_abs();
	and_exec(addr);
}

void op_3D(void) // AND abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	and_exec(addr);
}

void op_39(void) // AND abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	and_exec(addr);
}

void op_21(void) // AND (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	and_exec(addr);
}

void op_31(void) // AND (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	and_exec(addr);
}

// ORA family

void ora_exec(uint16_t addr)
{
	cpu.A = cpu.A | memory[addr];
	set_NZ_flags(cpu.A);
}

void op_09(void) // ORA imm
{
	uint16_t addr = cpu.PC++;
	ora_exec(addr);
}

void op_05(void) // ORA zpg
{
	uint16_t addr = memory[cpu.PC++];
	ora_exec(addr);
}

void op_15(void) // ORA zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	ora_exec(addr);
}

void op_0D(void) // ORA abs
{
	uint16_t addr = addr_abs();
	ora_exec(addr);
}

void op_1D(void) // ORA abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	ora_exec(addr);
}

void op_19(void) // ORA abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	ora_exec(addr);
}

void op_01(void) // ORA (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	ora_exec(addr);
}

void op_11(void) // ORA (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	ora_exec(addr);
}

// EOR family

void eor_exec(uint16_t addr)
{
	cpu.A = cpu.A ^ memory[addr];
	set_NZ_flags(cpu.A);
}

void op_49(void) // EOR imm
{
	uint16_t addr = cpu.PC++;
	eor_exec(addr);
}

void op_45(void) // EOR zpg
{
	uint16_t addr = memory[cpu.PC++];
	eor_exec(addr);
}

void op_55(void) // EOR zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	eor_exec(addr);
}

void op_4D(void) // EOR abs
{
	uint16_t addr = addr_abs();
	eor_exec(addr);
}

void op_5D(void) // EOR abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	eor_exec(addr);
}

void op_59(void) // EOR abs, Y
{
	uint16_t addr = addr_abs() + cpu.Y;
	eor_exec(addr);
}

void op_41(void) // EOR (indirect, X)
{
	uint16_t addr = addr_indexed_indirect();
	eor_exec(addr);
}

void op_51(void) // EOR (indirect), Y
{
	uint16_t addr = addr_indirect_indexed();
	eor_exec(addr);
}

// ROL/ROR family

void rol_exec(uint16_t addr)
{
	uint8_t result = (memory[addr] << 1) + cpu.C;
	cpu.C = memory[addr] >> 7;
	memory[addr] = result;
	set_NZ_flags(memory[addr]);
}

void op_2A(void) // ROL acc
{
	uint8_t result = (cpu.A << 1) + cpu.C;
	cpu.C = cpu.A >> 7;
	cpu.A = result;
	set_NZ_flags(cpu.A);

}

void op_26(void) // ROL zpg
{
	uint16_t addr = memory[cpu.PC++];
	rol_exec(addr);
}

void op_36(void) // ROL zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	rol_exec(addr);
}

void op_2E(void) // ROL abs
{
	uint16_t addr = addr_abs();
	rol_exec(addr);
}

void op_3E(void) // ROL abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	rol_exec(addr);
}

void ror_exec(uint16_t addr)
{
	uint8_t result = (memory[addr] >> 1) + (cpu.C << 7);
	cpu.C = memory[addr] & 0x1;
	memory[addr] = result;
	set_NZ_flags(memory[addr]);
}

void op_6A(void) // ROR acc
{
	uint8_t result = (cpu.A >> 1) + (cpu.C << 7);
	cpu.C = cpu.A & 0x1;
	cpu.A = result;
	set_NZ_flags(cpu.A);

}

void op_66(void) // ROR zpg
{
	uint16_t addr = memory[cpu.PC++];
	ror_exec(addr);
}

void op_76(void) // ROR zpg, X
{
	uint16_t addr = (memory[cpu.PC++] + cpu.X) & 0xFF;
	ror_exec(addr);
}

void op_6E(void) // ROR abs
{
	uint16_t addr = addr_abs();
	ror_exec(addr);
}

void op_7E(void) // ROR abs, X
{
	uint16_t addr = addr_abs() + cpu.X;
	ror_exec(addr);
}

void op_unk(void) // *Not implemented" or "Illegal instruction"
{
	printf("Unknown opcode - 0x%x at 0x%x\n", memory[cpu.PC - 1], cpu.PC - 1);
	exit(1);
}


op_handler op_hand[256];

void op_hand_init(void)
{
	for(int i = 0; i < 256; i++)
	{
		op_hand[i] = op_unk;
	}

	op_hand[0xA9] = op_A9;
	op_hand[0xA5] = op_A5;
	op_hand[0xB5] = op_B5;
	op_hand[0xAD] = op_AD;
	op_hand[0xBD] = op_BD;
	op_hand[0xB9] = op_B9;
	op_hand[0xA1] = op_A1;
	op_hand[0xB1] = op_B1;
	op_hand[0xA2] = op_A2;
	op_hand[0xA6] = op_A6;
	op_hand[0xB6] = op_B6;
	op_hand[0xAE] = op_AE;
	op_hand[0xBE] = op_BE;
	op_hand[0xA0] = op_A0;
	op_hand[0xA4] = op_A4;
	op_hand[0xB4] = op_B4;
	op_hand[0xAC] = op_AC;
	op_hand[0xBC] = op_BC;
	op_hand[0x4C] = op_4C;
	op_hand[0x85] = op_85;
	op_hand[0x95] = op_95;
	op_hand[0x8D] = op_8D;
	op_hand[0x9D] = op_9D;
	op_hand[0x99] = op_99;
	op_hand[0x81] = op_81;
	op_hand[0x91] = op_91;
	op_hand[0x86] = op_86;
	op_hand[0x96] = op_96;
	op_hand[0x8E] = op_8E;
	op_hand[0x84] = op_84;
	op_hand[0x94] = op_94;
	op_hand[0x8C] = op_8C;
	op_hand[0xC8] = op_C8;
	op_hand[0xE8] = op_E8;
	op_hand[0x88] = op_88;
	op_hand[0xCA] = op_CA;
	op_hand[0xC6] = op_C6;
	op_hand[0xD6] = op_D6;
	op_hand[0xCE] = op_CE;
	op_hand[0xDE] = op_DE;
	op_hand[0xE6] = op_E6;
	op_hand[0xF6] = op_F6;
	op_hand[0xEE] = op_EE;
	op_hand[0xFE] = op_FE;
	op_hand[0x18] = op_18;
	op_hand[0x38] = op_38;
	op_hand[0x58] = op_58;
	op_hand[0x61] = op_61;
	op_hand[0x65] = op_65;
	op_hand[0x69] = op_69;
	op_hand[0x6D] = op_6D;
	op_hand[0x71] = op_71;
	op_hand[0x75] = op_75;
	op_hand[0x78] = op_78;
	op_hand[0x79] = op_79;
	op_hand[0x7D] = op_7D;
	op_hand[0xB8] = op_B8;
	op_hand[0xD8] = op_D8;
	op_hand[0xE1] = op_E1;
	op_hand[0xE5] = op_E5;
	op_hand[0xE9] = op_E9;
	op_hand[0xED] = op_ED;
	op_hand[0xF1] = op_F1;
	op_hand[0xF5] = op_F5;
	op_hand[0xF8] = op_F8;
	op_hand[0xF9] = op_F9;
	op_hand[0xFD] = op_FD;
	op_hand[0x00] = op_00;
	op_hand[0xC9] = op_C9;
	op_hand[0xC0] = op_C0;
	op_hand[0xC1] = op_C1;
	op_hand[0xC4] = op_C4;
	op_hand[0xC5] = op_C5;
	op_hand[0xCC] = op_CC;
	op_hand[0xCD] = op_CD;
	op_hand[0xD1] = op_D1;
	op_hand[0xD5] = op_D5;
	op_hand[0xD9] = op_D9;
	op_hand[0xDD] = op_DD;
	op_hand[0xE0] = op_E0;
	op_hand[0xE4] = op_E4;
	op_hand[0xEC] = op_EC;
	op_hand[0x20] = op_20;
	op_hand[0x60] = op_60;
	op_hand[0x6C] = op_6C;
	op_hand[0x10] = op_10;
	op_hand[0x30] = op_30;
	op_hand[0x50] = op_50;
	op_hand[0x70] = op_70;
	op_hand[0x90] = op_90;
	op_hand[0xB0] = op_B0;
	op_hand[0xD0] = op_D0;
	op_hand[0xF0] = op_F0;
	op_hand[0x06] = op_06;
	op_hand[0x0A] = op_0A;
	op_hand[0x0E] = op_0E;
	op_hand[0x16] = op_16;
	op_hand[0x1E] = op_1E;
	op_hand[0x46] = op_46;
	op_hand[0x4A] = op_4A;
	op_hand[0x4E] = op_4E;
	op_hand[0x56] = op_56;
	op_hand[0x5E] = op_5E;
	op_hand[0x08] = op_08;
	op_hand[0x28] = op_28;
	op_hand[0x48] = op_48;
	op_hand[0x68] = op_68;
	op_hand[0x24] = op_24;
	op_hand[0x2C] = op_2C;
	op_hand[0x8A] = op_8A;
	op_hand[0x98] = op_98;
	op_hand[0x9A] = op_9A;
	op_hand[0xA8] = op_A8;
	op_hand[0xAA] = op_AA;
	op_hand[0xBA] = op_BA;
	op_hand[0x01] = op_01;
	op_hand[0x05] = op_05;
	op_hand[0x09] = op_09;
	op_hand[0x0D] = op_0D;
	op_hand[0x11] = op_11;
	op_hand[0x15] = op_15;
	op_hand[0x19] = op_19;
	op_hand[0x1D] = op_1D;
	op_hand[0x21] = op_21;
	op_hand[0x25] = op_25;
	op_hand[0x29] = op_29;
	op_hand[0x2D] = op_2D;
	op_hand[0x31] = op_31;
	op_hand[0x35] = op_35;
	op_hand[0x39] = op_39;
	op_hand[0x3D] = op_3D;
	op_hand[0x41] = op_41;
	op_hand[0x45] = op_45;
	op_hand[0x49] = op_49;
	op_hand[0x4D] = op_4D;
	op_hand[0x51] = op_51;
	op_hand[0x55] = op_55;
	op_hand[0x59] = op_59;
	op_hand[0x5D] = op_5D;
	op_hand[0xEA] = op_EA;
	op_hand[0x26] = op_26;
	op_hand[0x2A] = op_2A;
	op_hand[0x2E] = op_2E;
	op_hand[0x36] = op_36;
	op_hand[0x3E] = op_3E;
	op_hand[0x66] = op_66;
	op_hand[0x6A] = op_6A;
	op_hand[0x6E] = op_6E;
	op_hand[0x76] = op_76;
	op_hand[0x7E] = op_7E;
	op_hand[0x40] = op_40;
}

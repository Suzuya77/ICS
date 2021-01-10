#include "cpu/exec.h"

#define BINNARY_NUM_ZERO 0B0000000
#define BINNARY_NUM_ONE 0B0000001
#define BINNARY_NUM_THIRTY_TWO 0B0100000

//U-type
make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(auipc){
	rtl_add(&id_dest->val, &cpu.pc, &id_src->val);
	rtl_sr(id_dest->reg, &id_dest->val,  4);

	print_asm_template2(auipc);
}

//I-type
make_EHelper(addi){
	rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
	rtl_sr(id_dest->reg, &id_dest->val, 4);

	print_asm_template2(addi);
}

make_EHelper(slli){
	rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
	rtl_sr(id_dest->reg, &id_dest->val, 4);

	print_asm_template2(slli);
}


make_EHelper(slti){
	rtl_li(&id_dest->val, interpret_relop(RELOP_LT, id_src->val, id_src2->val));
	rtl_sr(id_dest->reg, &id_dest->val, 4);

	print_asm_template2(slti);
}

make_EHelper(sltiu){
	rtl_li(&id_dest->val, interpret_relop(RELOP_LTU, id_src->val, id_src2->val));
	rtl_sr(id_dest->reg, &id_dest->val, 4);

	print_asm_template2(sltiu); 
}

make_EHelper(xori){
	rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
  	rtl_sr(id_dest->reg, &id_dest->val, 4);

  	print_asm_template2(xori); 
}

make_EHelper(srli_srai){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template2(srli);
		return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_THIRTY_TWO)
	{
		rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template2(srai);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

make_EHelper(ori){
	rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
  	rtl_sr(id_dest->reg, &id_dest->val, 4);

  	print_asm_template2(ori); 
}

make_EHelper(andi){
	rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
  	rtl_sr(id_dest->reg, &id_dest->val, 4);

  	print_asm_template2(andi); 
}

/* R-type */
/* add ssub */
make_EHelper(add_sub){
	if (decinfo.isa.instr.funct7 ==  BINNARY_NUM_ZERO)
	{
		rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(add);
		return;
	}

	if (decinfo.isa.instr.funct7 ==  BINNARY_NUM_THIRTY_TWO)
	{
		rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(sub);
		return;
	}

	if (decinfo.isa.instr.funct7 ==  BINNARY_NUM_ONE)
	{
		rtl_imul_lo(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(add);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

make_EHelper(sll){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(sll);
		return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ONE)
	{
		rtl_imul_hi(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(mulh);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

make_EHelper(slt){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_li(&id_dest->val, interpret_relop(RELOP_LT, id_src->val, id_src2->val));
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(slt);
		return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ONE)
	{
		rtl_mul_hsu(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(mulhsu);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

make_EHelper(sltu){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_li(&id_dest->val, interpret_relop(RELOP_LTU, id_src->val, id_src2->val));
		rtl_sr(id_dest->reg, &id_dest->val, 4);

      	print_asm_template3(sltu);
      	return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ONE)
	{
		rtl_mul_hi(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

      	print_asm_template3(mulhu);
      	return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

make_EHelper(xor){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(xor);
		return;
	}


	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ONE)
	{
		rtl_idiv_q(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(div);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

make_EHelper(srl_sra){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(srl);
		return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ONE)
	{
		rtl_div_q(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(divu);
		return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_THIRTY_TWO)
	{
		rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(sra);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

make_EHelper(or){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(srl);
		return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ONE)
	{
		rtl_idiv_r(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(rem);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);

}

make_EHelper(and){
	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ZERO)
	{
		rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(and);
		return;
	}

	if (decinfo.isa.instr.funct7 == BINNARY_NUM_ONE)
	{
		rtl_div_r(&id_dest->val, &id_src->val, &id_src2->val);
		rtl_sr(id_dest->reg, &id_dest->val, 4);

		print_asm_template3(remu);
		return;
	}

	printf("Unrecognized Instruction: %x\n", decinfo.isa.instr.funct7);
	assert(0);
}

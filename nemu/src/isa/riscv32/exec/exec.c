#include "cpu/exec.h"
#include "all-instr.h"

static OpcodeEntry load_table [8] = {
  EMPTY, EMPTY, EXW(ld, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry store_table [8] = {
  EMPTY, EMPTY, EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry i_table [8] = {
  EX(addi), // 000 addi
  EX(slli), // 001 slli
  EX(slti), // 010 slti
  EX(sltiu), // 011 sltiu
  EX(xori), // 100 xori
  EX(srli_srai), // 101 srai
  EX(ori), // 110 ori
  EX(andi) // 111 andi
};

static make_EHelper(i){
  decinfo.width = i_table[decinfo.isa.instr.funct3].width;
  idex(pc, &i_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry branch_table [8] = {
  EX(beq), // 000
  EX(bne), // 001
  EMPTY, 
  EMPTY, 
  EX(blt), // 100
  EX(bge), // 101
  EX(bltu), // 110
  EX(bgeu) // 111
};

static make_EHelper(branch){
  decinfo.width = branch_table[decinfo.isa.instr.funct3].width;
  idex(pc, &branch_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry r_table [8] = {
  EX(add_sub), // 000
  EX(sll), // 001
  EX(slt), // 010
  EX(sltu), // 011
  EX(xor), // 100
  EX(srl_sra), // 101
  EX(or), // 110
  EX(and) // 111
};

static make_EHelper(r){
  decinfo.width = r_table[decinfo.isa.instr.funct3].width;
  idex(pc, &r_table[decinfo.isa.instr.funct3]);
}

static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, i), IDEX(U, auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R, r), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B, branch), IDEX(I, jalr), EX(nemu_trap), IDEX(J, jal), IDEX(Sys, system), EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}

#include "cpu/exec.h"

make_EHelper(lui);

make_EHelper(ld);
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);

make_EHelper(addi);
make_EHelper(slli);
make_EHelper(slti);
make_EHelper(sltiu);
make_EHelper(xori);
make_EHelper(srli_srai);
make_EHelper(ori);
make_EHelper(andi);

make_EHelper(auipc);

make_EHelper(jal);
make_EHelper(jalr);

make_EHelper(beq);
make_EHelper(bne);
make_EHelper(blt);
make_EHelper(bge);
make_EHelper(bltu);
make_EHelper(bgeu);

make_EHelper(add_sub);
make_EHelper(sll);
make_EHelper(slt);
make_EHelper(sltu);
make_EHelper(xor);
make_EHelper(srl_sra);
make_EHelper(or);
make_EHelper(and);

make_EHelper(system);
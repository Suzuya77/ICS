#include "cpu/exec.h"

extern void raise_intr(uint32_t, vaddr_t);
int32_t readcsr(int i){
	uint32_t result;
	switch(i){
	  case 0x105:
	  	result = decinfo.isa.stvec;
	  	break;
	  case 0x142:
      	result =  decinfo.isa.scause;
      	break;
	  case 0x100:
      	result =  decinfo.isa.sstatus;
      	break;
	  case 0x141:
      	result =  decinfo.isa.sepc;
      	break;
	  default:
	  	printf("Error Code: 0x%x\n", i);
      	assert(0);
	}

	return result;
}

void writecsr(int i, int32_t val){
	switch(i){
	  case 0x105:
	      decinfo.isa.stvec = val;
	      break;
	  case 0x142:
	      decinfo.isa.scause = val;
	      break;
	  case 0x100:
	      decinfo.isa.sstatus = val;
	      break;
	  case 0x141:
	      decinfo.isa.sepc = val;
	      break;
	  default:
      	  assert(0);
	}
}

make_EHelper(system){
	// printf("System Ex\n");
	t0 = 4;
  	rtl_add(&decinfo.jmp_pc, &t0, &decinfo.isa.sepc);

	switch(decinfo.isa.instr.funct3){
		case 0b0:
		    if((decinfo.isa.instr.val & ~(0x7f))==0){
		        raise_intr(reg_l(17), decinfo.seq_pc - 4);
		    }
		    else if(decinfo.isa.instr.val == 0x10200073){
		        decinfo.is_jmp = 1;
		    }
		    else{
		        assert(0);
		    }
		    break;

		case 0b001:
		    s0 = readcsr(decinfo.isa.instr.csr);
		    writecsr(decinfo.isa.instr.csr, id_src->val);
		    rtl_sr(id_dest->reg, &s0, 4);
		    break;
	    
		case 0b010:
		    s0 = readcsr(decinfo.isa.instr.csr);
		    writecsr(decinfo.isa.instr.csr, s0 | id_src->val);
		    rtl_sr(id_dest->reg, &s0, 4);
	    	break;
		default:
	    	assert(0);
	}
}

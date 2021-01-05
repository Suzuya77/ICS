#include "nemu.h"

#define REGISTER_NUMBER 32

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	int regNum = sizeof(regsl)/sizeof(regsl[0]);
  	rtlreg_t reg;
  	for(int i = 0; i < regNum; i++)
  	{
    	reg = reg_l(i);
    	printf("%-6s 0x%x %9d\n", reg_name(i, sizeof(reg)*8), reg, reg);
  	}
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
	*success = true;
	if (strcmp(s,"pc") == 0)
	{
		return cpu.pc;
	}
	else if (s[0] == '0')
	{
		return reg_l(0);
	}

	int i = 0;
	for (; i < REGISTER_NUMBER; ++i)
	{
		if (strcmp(regsl[i], s) == 0)
		{
			break;
		}
	}

	if (i < 32)
	{
		return reg_l(i);
	}

	*success = false;
  	return 0;
}

#pragma once
#include "GB_memory.h"

class GB_CPU {
public:
	void init();
	void CPU_Step();
	

private:

	GB_Byte Reg_A{ 0 };
	GB_Byte Reg_F{ 0 };

	GB_Byte Reg_B{ 0 };
	GB_Byte Reg_C{ 0 };
	
	GB_Byte Reg_D{ 0 };
	GB_Byte Reg_E{ 0 };

	GB_Byte Reg_H{ 0 };
	GB_Byte Reg_L{ 0 };

	GB_DoubleByte Reg_SP{ 0 };
	GB_DoubleByte Reg_PC{ 0 };

	bool Flag_Zero{ 0 };
	bool Flag_Subtract{ 0 };
	bool Flag_HalfCarry{ 0 };
	bool Flag_Carry{ 0 };

};

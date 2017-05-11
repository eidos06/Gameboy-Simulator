#pragma once
#include "GB_memory.h"
#include<array>
#include<functional>
using namespace std;
class GB_CPU {
public:
	GB_CPU(GB_memory &In_memory)
		:memory_(In_memory)
	{
		
		init();
	}
	void init();
	void CPU_Step();
	void Opcode_load();
	
private:
	
	GB_memory memory_;

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
	
	void GB_ADD(GB_Byte num1);
	void GB_ADC(GB_Byte num1);
	void GB_SUB(GB_Byte num1);
	void GB_SBC(GB_Byte num1);
	void GB_AND(GB_Byte num1);
	void GB_OR(GB_Byte num1);
	void GB_XOR(GB_Byte num1);
	void GB_CP(GB_Byte num1);
	void GB_INC(GB_Byte &num1);
	void GB_DEC(GB_Byte &num1);

	array<function<int()>, 0x100> GB_Opcode;

};

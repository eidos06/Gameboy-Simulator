#pragma once
#include "GB_memory.h"
#include "GB_Bits.h"
#include "GB_Const.h"
#include<array>
#include<functional>
using namespace std;


extern GB_memory memory_;


class GB_CPU {
public:
	
	void init();
	int CPU_Step();
	void Opcode_load();
	
	int time{ 0 };
	
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

	
	bool Flag_HALT{ 0 };
	bool Flag_INTERRUPT{ 0 };
	
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
	void GB_ADDHL(GB_DoubleByte num1,GB_DoubleByte num2);
	void GB_ADDSP();
	void GB_SWAP(GB_Byte &num1);
	void GB_RLC(GB_Byte &num1);
	void GB_RL(GB_Byte &num1);
	void GB_RRC(GB_Byte &num1);
	void GB_RR(GB_Byte &num1);
	void GB_SLA(GB_Byte &num1);
	void GB_SRA(GB_Byte &num1);
	void GB_SRL(GB_Byte &num1);

	void SetFlag(int flag, bool value) { SetBit(Reg_F, flag, value); }
	bool GetFlag(int flag) { return GetBit(Reg_F, flag); }
	array<function<int()>, 0x100> GB_Opcode;
	array<function<int()>, 0x100> GB_CBOpcode;
};

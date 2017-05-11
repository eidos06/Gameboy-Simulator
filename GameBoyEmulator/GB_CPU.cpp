#include "GB_CPU.h"

void GB_CPU::init()
{
	Reg_A = 0x01;
	Reg_F = 0xB0;
	Reg_B = 0x00;
	Reg_C = 0x13;
	Reg_D = 0x00;
	Reg_E = 0xD8;
	Reg_H = 0x01;
	Reg_L = 0x4D;
	Reg_PC = 0x0100;
	Reg_SP = 0xFFFE;
	Opcode_load();



}

void GB_CPU::CPU_Step() {


}



void GB_CPU::Opcode_load()
{
	auto Reg_AF = [&]()-> GB_DoubleByte {return (Reg_A << 8) | Reg_F; };
	auto Reg_BC = [&]()-> GB_DoubleByte {return (Reg_B << 8) | Reg_C; };
	auto Reg_DE = [&]()-> GB_DoubleByte {return (Reg_D << 8) | Reg_E; };
	auto Reg_HL = [&]()-> GB_DoubleByte {return (Reg_H << 8) | Reg_L; };
	
	//LD nn,n
	GB_Opcode[0x06] = [&]()->int {Reg_B = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x0E] = [&]()->int {Reg_C = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x16] = [&]()->int {Reg_D = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x1E] = [&]()->int {Reg_E = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x26] = [&]()->int {Reg_H = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x2E] = [&]()->int {Reg_L = memory_.ReadByte(Reg_PC++); return 8; };
	
	//LD r1,r2
	GB_Opcode[0x7F] = [&]()->int {Reg_A = Reg_A; return 4; };
	GB_Opcode[0x78] = [&]()->int {Reg_A = Reg_B; return 4; };
	GB_Opcode[0x79] = [&]()->int {Reg_A = Reg_C; return 4; };
	GB_Opcode[0x7A] = [&]()->int {Reg_A = Reg_D; return 4; };
	GB_Opcode[0x7B] = [&]()->int {Reg_A = Reg_E; return 4; };
	GB_Opcode[0x7C] = [&]()->int {Reg_A = Reg_H; return 4; };
	GB_Opcode[0x7D] = [&]()->int {Reg_A = Reg_L; return 4; };
	GB_Opcode[0x7E] = [&]()->int {Reg_A = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x40] = [&]()->int {Reg_B = Reg_B; return 4; };
	GB_Opcode[0x41] = [&]()->int {Reg_B = Reg_C; return 4; };
	GB_Opcode[0x42] = [&]()->int {Reg_B = Reg_D; return 4; };
	GB_Opcode[0x43] = [&]()->int {Reg_B = Reg_E; return 4; };
	GB_Opcode[0x44] = [&]()->int {Reg_B = Reg_H; return 4; };
	GB_Opcode[0x45] = [&]()->int {Reg_B = Reg_L; return 4; };
	GB_Opcode[0x46] = [&]()->int {Reg_B =memory_.ReadByte( Reg_HL()); return 8; };
	GB_Opcode[0x48] = [&]()->int {Reg_C = Reg_B; return 4; };
	GB_Opcode[0x49] = [&]()->int {Reg_C = Reg_C; return 4; };
	GB_Opcode[0x4A] = [&]()->int {Reg_C = Reg_D; return 4; };
	GB_Opcode[0x4B] = [&]()->int {Reg_C = Reg_E; return 4; };
	GB_Opcode[0x4C] = [&]()->int {Reg_C = Reg_H; return 4; };
	GB_Opcode[0x4D] = [&]()->int {Reg_C = Reg_L; return 4; };
	GB_Opcode[0x4E] = [&]()->int {Reg_C = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x50] = [&]()->int {Reg_D = Reg_B; return 4; };
	GB_Opcode[0x51] = [&]()->int {Reg_D = Reg_C; return 4; };
	GB_Opcode[0x52] = [&]()->int {Reg_D = Reg_D; return 4; };
	GB_Opcode[0x53] = [&]()->int {Reg_D = Reg_E; return 4; };
	GB_Opcode[0x54] = [&]()->int {Reg_D = Reg_H; return 4; };
	GB_Opcode[0x55] = [&]()->int {Reg_D = Reg_L; return 4; };
	GB_Opcode[0x56] = [&]()->int {Reg_D = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x58] = [&]()->int {Reg_E = Reg_B; return 4; };
	GB_Opcode[0x59] = [&]()->int {Reg_E = Reg_C; return 4; };
	GB_Opcode[0x5A] = [&]()->int {Reg_E = Reg_D; return 4; };
	GB_Opcode[0x5B] = [&]()->int {Reg_E = Reg_E; return 4; };
	GB_Opcode[0x5C] = [&]()->int {Reg_E = Reg_H; return 4; };
	GB_Opcode[0x5D] = [&]()->int {Reg_E = Reg_L; return 4; };
	GB_Opcode[0x5E] = [&]()->int {Reg_E = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x60] = [&]()->int {Reg_H = Reg_B; return 4; };
	GB_Opcode[0x61] = [&]()->int {Reg_H = Reg_C; return 4; };
	GB_Opcode[0x62] = [&]()->int {Reg_H = Reg_D; return 4; };
	GB_Opcode[0x63] = [&]()->int {Reg_H = Reg_E; return 4; };
	GB_Opcode[0x64] = [&]()->int {Reg_H = Reg_H; return 4; };
	GB_Opcode[0x65] = [&]()->int {Reg_H = Reg_L; return 4; };
	GB_Opcode[0x66] = [&]()->int {Reg_H = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x68] = [&]()->int {Reg_L = Reg_B; return 4; };
	GB_Opcode[0x69] = [&]()->int {Reg_L = Reg_C; return 4; };
	GB_Opcode[0x6A] = [&]()->int {Reg_L = Reg_D; return 4; };
	GB_Opcode[0x6B] = [&]()->int {Reg_L = Reg_E; return 4; };
	GB_Opcode[0x6C] = [&]()->int {Reg_L = Reg_H; return 4; };
	GB_Opcode[0x6D] = [&]()->int {Reg_L = Reg_L; return 4; };
	GB_Opcode[0x6E] = [&]()->int {Reg_L = memory_.ReadByte(Reg_HL()); return 4; };
	GB_Opcode[0x70] = [&]()->int {memory_.WriteByte(Reg_HL(),Reg_B); return 8; };
	GB_Opcode[0x71] = [&]()->int {memory_.WriteByte(Reg_HL(), Reg_C); return 8; };
	GB_Opcode[0x72] = [&]()->int {memory_.WriteByte(Reg_HL(), Reg_D); return 8; };
	GB_Opcode[0x73] = [&]()->int {memory_.WriteByte(Reg_HL(), Reg_E); return 8; };
	GB_Opcode[0x74] = [&]()->int {memory_.WriteByte(Reg_HL(), Reg_H); return 8; };
	GB_Opcode[0x75] = [&]()->int {memory_.WriteByte(Reg_HL(), Reg_L); return 8; };
	GB_Opcode[0x36] = [&]()->int {memory_.WriteByte(Reg_HL(), memory_.ReadByte(Reg_PC++)); return 12; };
	
	//LD A,n
	GB_Opcode[0x7F] = [&]()->int {Reg_A = Reg_A; return 4; };
	GB_Opcode[0x78] = [&]()->int {Reg_A = Reg_B; return 4; };
	GB_Opcode[0x79] = [&]()->int {Reg_A = Reg_C; return 4; };
	GB_Opcode[0x7A] = [&]()->int {Reg_A = Reg_D; return 4; };
	GB_Opcode[0x7B] = [&]()->int {Reg_A = Reg_E; return 4; };
	GB_Opcode[0x7C] = [&]()->int {Reg_A = Reg_H; return 4; };
	GB_Opcode[0x7D] = [&]()->int {Reg_A = Reg_L; return 4; };
	GB_Opcode[0x0A] = [&]()->int {Reg_A = memory_.ReadByte(Reg_BC()); return 8; };
	GB_Opcode[0x1A] = [&]()->int {Reg_A = memory_.ReadByte(Reg_DE()); return 8; };
	GB_Opcode[0x7E] = [&]()->int {Reg_A = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0xFA] = [&]()->int {Reg_A = memory_.ReadByte(Reg_PC); Reg_PC += 2; return 16; };
	GB_Opcode[0x3E] = [&]()->int {Reg_A = memory_.ReadByte(Reg_PC++); return 8; };

	//LD n,A
	GB_Opcode[0x7F] = [&]()->int {Reg_A = Reg_A; return 4; };
	GB_Opcode[0x47] = [&]()->int {Reg_B = Reg_A; return 4; };
	GB_Opcode[0x4F] = [&]()->int {Reg_C = Reg_A; return 4; };
	GB_Opcode[0x57] = [&]()->int {Reg_D = Reg_A; return 4; };
	GB_Opcode[0x5F] = [&]()->int {Reg_E = Reg_A; return 4; };
	GB_Opcode[0x67] = [&]()->int {Reg_H = Reg_A; return 4; };
	GB_Opcode[0x6F] = [&]()->int {Reg_L = Reg_A; return 4; };
	GB_Opcode[0x02] = [&]()->int {memory_.WriteByte(Reg_BC(),Reg_A); return 8; };
	GB_Opcode[0x12] = [&]()->int {memory_.WriteByte(Reg_DE(), Reg_A); return 8; };
	GB_Opcode[0x77] = [&]()->int {memory_.WriteByte(Reg_HL(), Reg_A); return 8; };
	GB_Opcode[0xEA] = [&]()->int {memory_.WriteByte(memory_.ReadDoubleByte(Reg_PC), Reg_A); Reg_PC += 2; return 16; };

	//LD A,(C)
	GB_Opcode[0xF2] = [&]()->int {Reg_A = memory_.ReadByte(0xFF00 + Reg_C); return 8; }; 

	//LD (C),A
	GB_Opcode[0xE2] = [&]()->int {memory_.WriteByte(0xFF00 + Reg_C, Reg_A); return 8; };

	//LDD A,(HL)

	GB_Opcode[0x3A] = [&]()->int {GB_Opcode[0x7E](); GB_Opcode[0x2B]; return 8; };

	//LDD (HL),A
	GB_Opcode[0x32] = [&]()->int {GB_Opcode[0x77](); GB_Opcode[0x2B]; return 8; };

	//LDI A,(HL)
	GB_Opcode[0x2A] = [&]()->int {GB_Opcode[0x7E](); GB_Opcode[0x23]; return 8; };
	
	//LDI (HL),A
	GB_Opcode[0x22] = [&]()->int {GB_Opcode[0x77](); GB_Opcode[0x23]; return 8; };

	//LDH (n),A
	GB_Opcode[0xE0] = [&]()->int {memory_.WriteByte(0xFF00 + memory_.ReadByte(Reg_PC++), Reg_A); return 12; };

	//LDH A,(n)
	GB_Opcode[0xF0] = [&]()->int {Reg_A = memory_.ReadByte(0xFF00 + memory_.ReadByte(Reg_PC++)); return 12; };

	//LD n,nn
	GB_Opcode[0x01] = [&]()->int {Reg_C = memory_.ReadByte(Reg_PC++); Reg_B = memory_.ReadByte(Reg_PC++); return 12; };
	GB_Opcode[0x11] = [&]()->int {Reg_E = memory_.ReadByte(Reg_PC++); Reg_D = memory_.ReadByte(Reg_PC++); return 12; };
	GB_Opcode[0x21] = [&]()->int {Reg_L = memory_.ReadByte(Reg_PC++); Reg_H = memory_.ReadByte(Reg_PC++); return 12; };
	GB_Opcode[0x31] = [&]()->int {
		GB_Byte low=memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++); 
		Reg_SP = high << 8 | low;
		return 12; };


	//LD SP,HL
	GB_Opcode[0xF9] = [&]()->int {Reg_SP = Reg_HL(); return 8; };

	//LDHL SP,n
	//GB_Opcode[0xF8] = [&]()->int {};

	//LD (nn),SP
	GB_Opcode[0x08] = [&]()->int {memory_.WriteDoubleByte(Reg_PC, Reg_SP); Reg_PC += 2; return 20; };

	//PUSH nn
	GB_Opcode[0xF5] = [&]()->int {Reg_SP = Reg_SP - 2; memory_.WriteDoubleByte(Reg_SP, Reg_AF()); return 16; };
	GB_Opcode[0xC5] = [&]()->int {Reg_SP = Reg_SP - 2; memory_.WriteDoubleByte(Reg_SP, Reg_BC()); return 16; };
	GB_Opcode[0xD5] = [&]()->int {Reg_SP = Reg_SP - 2; memory_.WriteDoubleByte(Reg_SP, Reg_DE()); return 16; };
	GB_Opcode[0xE5] = [&]()->int {Reg_SP = Reg_SP - 2; memory_.WriteDoubleByte(Reg_SP, Reg_HL()); return 16; };
	//POP nn
	GB_Opcode[0xF1] = [&]()->int {Reg_F = memory_.ReadByte(Reg_SP); Reg_A = memory_.ReadByte(Reg_SP + 1); Reg_SP += 2; return 16; };
	GB_Opcode[0xC1] = [&]()->int {Reg_C = memory_.ReadByte(Reg_SP); Reg_B = memory_.ReadByte(Reg_SP + 1); Reg_SP += 2; return 16; };
	GB_Opcode[0xD1] = [&]()->int {Reg_E = memory_.ReadByte(Reg_SP); Reg_D = memory_.ReadByte(Reg_SP + 1); Reg_SP += 2; return 16; };
	GB_Opcode[0xE1] = [&]()->int {Reg_L = memory_.ReadByte(Reg_SP); Reg_H = memory_.ReadByte(Reg_SP + 1); Reg_SP += 2; return 16; };
	//ADD A,n

 	GB_Opcode[0x87] = [&]()->int {GB_ADD(Reg_A); return 4; };
	GB_Opcode[0x80] = [&]()->int {GB_ADD(Reg_B); return 4; };
	GB_Opcode[0x81] = [&]()->int {GB_ADD(Reg_C); return 4; };
	GB_Opcode[0x82] = [&]()->int {GB_ADD(Reg_D); return 4; };
	GB_Opcode[0x83] = [&]()->int {GB_ADD(Reg_E); return 4; };
	GB_Opcode[0x84] = [&]()->int {GB_ADD(Reg_H); return 4; };
	GB_Opcode[0x85] = [&]()->int {GB_ADD(Reg_L); return 4; };
	GB_Opcode[0x86] = [&]()->int {GB_ADD(memory_.ReadByte(Reg_HL())) ; return 8; };
	GB_Opcode[0xC6] = [&]()->int {GB_ADD(memory_.ReadByte(Reg_PC++)); return 8; };



	//ADC A,n
	GB_Opcode[0x8F] = [&]()->int {GB_ADC(Reg_A); return 4; };
	GB_Opcode[0x88] = [&]()->int {GB_ADC(Reg_B); return 4; };
	GB_Opcode[0x89] = [&]()->int {GB_ADC(Reg_C); return 4; };
	GB_Opcode[0x8A] = [&]()->int {GB_ADC(Reg_D); return 4; };
	GB_Opcode[0x8B] = [&]()->int {GB_ADC(Reg_E); return 4; };
	GB_Opcode[0x8C] = [&]()->int {GB_ADC(Reg_H); return 4; };
	GB_Opcode[0x8D] = [&]()->int {GB_ADC(Reg_L); return 4; };
	GB_Opcode[0x8E] = [&]()->int {GB_ADC(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xCE] = [&]()->int {GB_ADC(memory_.ReadByte(Reg_PC++)); return 8; };


	//SUB n

	GB_Opcode[0x97] = [&]()->int {GB_SUB(Reg_A); return 4; };
	GB_Opcode[0x90] = [&]()->int {GB_SUB(Reg_B); return 4; };
	GB_Opcode[0x91] = [&]()->int {GB_SUB(Reg_C); return 4; };
	GB_Opcode[0x92] = [&]()->int {GB_SUB(Reg_D); return 4; };
	GB_Opcode[0x93] = [&]()->int {GB_SUB(Reg_E); return 4; };
	GB_Opcode[0x94] = [&]()->int {GB_SUB(Reg_H); return 4; };
	GB_Opcode[0x95] = [&]()->int {GB_SUB(Reg_L); return 4; };
	GB_Opcode[0x96] = [&]()->int {GB_SUB(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xD6] = [&]()->int {GB_SUB(memory_.ReadByte(Reg_PC++)); return 8; };



	//SBC A,n

	GB_Opcode[0x9F] = [&]()->int {GB_SBC(Reg_A); return 4; };
	GB_Opcode[0x98] = [&]()->int {GB_SBC(Reg_B); return 4; };
	GB_Opcode[0x99] = [&]()->int {GB_SBC(Reg_C); return 4; };
	GB_Opcode[0x9A] = [&]()->int {GB_SBC(Reg_D); return 4; };
	GB_Opcode[0x9B] = [&]()->int {GB_SBC(Reg_E); return 4; };
	GB_Opcode[0x9C] = [&]()->int {GB_SBC(Reg_H); return 4; };
	GB_Opcode[0x9D] = [&]()->int {GB_SBC(Reg_L); return 4; };
	GB_Opcode[0x9E] = [&]()->int {GB_SBC(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xDE] = [&]()->int {GB_SBC(memory_.ReadByte(Reg_PC++)); return 8; };

	//AND n

	GB_Opcode[0xA7] = [&]()->int {GB_AND(Reg_A); return 4; };
	GB_Opcode[0xA0] = [&]()->int {GB_AND(Reg_B); return 4; };
	GB_Opcode[0xA1] = [&]()->int {GB_AND(Reg_C); return 4; };
	GB_Opcode[0xA2] = [&]()->int {GB_AND(Reg_D); return 4; };
	GB_Opcode[0xA3] = [&]()->int {GB_AND(Reg_E); return 4; };
	GB_Opcode[0xA4] = [&]()->int {GB_AND(Reg_H); return 4; };
	GB_Opcode[0xA5] = [&]()->int {GB_AND(Reg_L) return 4; };
	GB_Opcode[0xA6] = [&]()->int {GB_AND(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xE6] = [&]()->int {GB_AND(memory_.ReadByte(Reg_PC++)); return 8; };
	

	//OR n

	GB_Opcode[0xB7] = [&]()->int {GB_OR(Reg_A); return 4; };
	GB_Opcode[0xB0] = [&]()->int {GB_OR(Reg_B); return 4; };
	GB_Opcode[0xB1] = [&]()->int {GB_OR(Reg_C); return 4; };
	GB_Opcode[0xB2] = [&]()->int {GB_OR(Reg_D); return 4; };
	GB_Opcode[0xB3] = [&]()->int {GB_OR(Reg_E); return 4; };
	GB_Opcode[0xB4] = [&]()->int {GB_OR(Reg_H); return 4; };
	GB_Opcode[0xB5] = [&]()->int {GB_OR(Reg_L); return 4; };
	GB_Opcode[0xB6] = [&]()->int {GB_OR(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xF6] = [&]()->int {GB_OR(memory_.ReadByte(Reg_PC++)); return 8; };



	//XOR n

	GB_Opcode[0xAF] = [&]()->int {GB_XOR(Reg_A); return 4; };
	GB_Opcode[0xA8] = [&]()->int {GB_XOR(Reg_B); return 4; };
	GB_Opcode[0xA9] = [&]()->int {GB_XOR(Reg_C); return 4; };
	GB_Opcode[0xAA] = [&]()->int {GB_XOR(Reg_D); return 4; };
	GB_Opcode[0xAB] = [&]()->int {GB_XOR(Reg_E); return 4; };
	GB_Opcode[0xAC] = [&]()->int {GB_XOR(Reg_H); return 4; };
	GB_Opcode[0xAD] = [&]()->int {GB_XOR(Reg_L); return 4; };
	GB_Opcode[0xAE] = [&]()->int {GB_XOR(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xEE] = [&]()->int {GB_XOR(memory_.ReadByte(Reg_PC++)); return 8; };



	//	CP n
	GB_Opcode[0xBF] = [&]()->int {GB_CP(Reg_A); return 4; };
	GB_Opcode[0xB8] = [&]()->int {GB_CP(Reg_B); return 4; };
	GB_Opcode[0xB9] = [&]()->int {GB_CP(Reg_C); return 4; };
	GB_Opcode[0xBA] = [&]()->int {GB_CP(Reg_D); return 4; };
	GB_Opcode[0xBB] = [&]()->int {GB_CP(Reg_E); return 4; };
	GB_Opcode[0xBC] = [&]()->int {GB_CP(Reg_H); return 4; };
	GB_Opcode[0xBD] = [&]()->int {GB_CP(Reg_L); return 4; };
	GB_Opcode[0xBE] = [&]()->int {GB_CP(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xFE] = [&]()->int {GB_CP(memory_.ReadByte(Reg_PC++)); return 8; };


	//INC n

	GB_Opcode[0x3C] = [&]()->int {A++; return 4; };

	GB_Opcode[0x04] = [&]()->int {B++; return 4; };

	GB_Opcode[0x0C] = [&]()->int {C++; return 4; };

	GB_Opcode[0x14] = [&]()->int {D++; return 4; };

	GB_Opcode[0x1C] = [&]()->int {E++; return 4; };

	GB_Opcode[0x24] = [&]()->int {H++; return 4; };

	GB_Opcode[0x2C] = [&]()->int {L++; return 4; };

	GB_Opcode[0x34] = [&]()->int {HL()++; return 12; };



	//DEC n

	GB_Opcode[0x3D] = [&]()->int {A--; return 4; };

	GB_Opcode[0x05] = [&]()->int {B--; return 4; };

	GB_Opcode[0x0D] = [&]()->int {C--; return 4; };

	GB_Opcode[0x15] = [&]()->int {D--; return 4; };

	GB_Opcode[0x1D] = [&]()->int {E--; return 4; };

	GB_Opcode[0x25] = [&]()->int {H--; return 4; };

	GB_Opcode[0x2D] = [&]()->int {L--; return 4; };

	GB_Opcode[0x35] = [&]()->int {HL()--; return 12; };



	//ADD HL,n

	GB_Opcode[0x09] = [&]()->int {HL() = HL() + BC; return 8; };

	GB_Opcode[0x19] = [&]()->int {HL() = HL() + DE; return 8; };

	GB_Opcode[0x29] = [&]()->int {HL() = HL() + HL(); return 8; };

	GB_Opcode[0x39] = [&]()->int {HL() = HL() + SP; return 8; };



	//ADD SP,n

	GB_Opcode[0xE8] = [&]()->int {SP = SP + #; return 16; };



	//INC nn

	GB_Opcode[0x03] = [&]()->int {BC++; return 8; };

	GB_Opcode[0x13] = [&]()->int {DE++; return 8; };

	GB_Opcode[0x23] = [&]()->int {HL()++; return 8; };

	GB_Opcode[0x33] = [&]()->int {SP++; return 8; };



	//DEC nn

	GB_Opcode[0x0B] = [&]()->int {BC--; return 8; };

	GB_Opcode[0x1B] = [&]()->int {DE--; return 8; };

	GB_Opcode[0x2B] = [&]()->int {HL()--; return 8; };

	GB_Opcode[0x3B] = [&]()->int {SP--; return 8; };



	//SWAP n

	array<function<int()>, 0x100> GB_CBOpcode;

	GB_Opcode[0xCB] = [&]()->int {return GB_CBOpcode[Reg_PC++](); };

	GB_CBOpcode[0x37] = [&]()->int {

		GB_Byte temp = Reg_A & 0x0f;

		Reg_A >>= 4;

		Reg_A |= (temp << 4);

		return 8; };

	GB_CBOpcode[0x30] = [&]()->int {

		GB_Byte temp = Reg_B & 0x0f;

		Reg_B >>= 4;

		Reg_B |= (temp << 4);

		return 8; };

	GB_CBOpcode[0x31] = [&]()->int {

		GB_Byte temp = Reg_C & 0x0f;

		Reg_C >>= 4;

		Reg_C |= (temp << 4);

		return 8; };

	GB_CBOpcode[0x32] = [&]()->int {

		GB_Byte temp = Reg_D & 0x0f;

		Reg_D >>= 4;

		Reg_D |= (temp << 4);

		return 8; };

	GB_CBOpcode[0x33] = [&]()->int {

		GB_Byte temp = Reg_E & 0x0f;

		Reg_E >>= 4;

		Reg_E |= (temp << 4);

		return 8; };

	GB_CBOpcode[0x34] = [&]()->int {

		GB_Byte temp = Reg_H & 0x0f;

		Reg_H >>= 4;

		Reg_H |= (temp << 4);

		return 8; };

	GB_CBOpcode[0x35] = [&]()->int {

		GB_Byte temp = Reg_L & 0x0f;

		Reg_L >>= 4;

		Reg_L |= (temp << 4);

		return 8; };

	GB_CBOpcode[0x36] = [&]()->int {

		GB_Byte temp = Reg_HL() & 0x0f;

		Reg_HL() >>= 4;

		Reg_HL() |= (temp << 4);

		return 8; };




	





}

void GB_CPU::GB_ADD(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A + num1;
	
	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 0;
	Flag_HalfCarry = (tmp^Reg_A^num1) & 0x10;
	Flag_Carry= (tmp^Reg_A^num1) & 0x100;
	
	Reg_A = tmp&0xFF;
}

void GB_CPU::GB_ADC(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A + num1 +Flag_Carry;

	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 0;
	Flag_HalfCarry = (tmp^Reg_A^num1) & 0x10;
	Flag_Carry = (tmp^Reg_A^num1) & 0x100;

	Reg_A = tmp & 0xFF;
}

void GB_CPU::GB_SUB(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A - num1;

	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 1;
	Flag_HalfCarry = (tmp^Reg_A^num1) & 0x10;
	Flag_Carry = (tmp^Reg_A^num1) & 0x100;

	Reg_A = tmp & 0xFF;
}

void GB_CPU::GB_SBC(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A - num1 - Flag_Carry;

	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 1;
	Flag_HalfCarry = (tmp^Reg_A^num1) & 0x10;
	Flag_Carry = (tmp^Reg_A^num1) & 0x100;

	Reg_A = tmp & 0xFF;
}

void GB_CPU::GB_AND(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A & num1;

	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 0;
	Flag_HalfCarry = 1;
	Flag_Carry = 0;

	Reg_A = tmp;
}

void GB_CPU::GB_OR(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A | num1;

	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 0;
	Flag_HalfCarry = 0;
	Flag_Carry = 0;

	Reg_A = tmp;
}

void GB_CPU::GB_XOR(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A ^ num1;

	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 0;
	Flag_HalfCarry = 0;
	Flag_Carry = 0;

	Reg_A = tmp;
}

void GB_CPU::GB_CP(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A - num1;

	Flag_Zero = (tmp == 0 || tmp==0x100);
	Flag_Subtract = 1;
	Flag_HalfCarry=(tmp^Reg_A^num1) & 0x10;
	Flag_Carry = (tmp^Reg_A^num1) & 0x100;


}

void GB_CPU::GB_INC(GB_Byte &num1)
{
	GB_DoubleByte tmp = num1 + 1;

	Flag_Zero = (tmp == 0 || tmp == 0x100);
	Flag_Subtract = 0;
	Flag_HalfCarry = (tmp^Reg_A^num1) & 0x10;

	num1 = tmp;

}
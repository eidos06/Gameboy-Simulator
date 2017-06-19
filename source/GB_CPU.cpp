#include "GB_CPU.h"
#include <iostream>
#include <fstream>
using namespace std;
#ifdef DEBUG
ofstream outfile;
int counts = 0;
#endif // DEBUG

void GB_CPU::init()
{
	//init registers 
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
	//load opcodes in function array
	Opcode_load();
#ifdef DEBUG
	outfile.open("opcode1.txt");
#endif // DEBUG

}

int GB_CPU::CPU_Step() {
	//default timing increase
	int AddTime = 4;
	//load interrupts
	GB_Byte InterruptEnable = memory_.ReadByte(0xFFFF);//interrupt are enabled in system?
	GB_Byte InterruptFlag = memory_.ReadByte(IF_ADDRESS);//interrupt occured?
	bool Interrupts = InterruptEnable & InterruptFlag;//Has interrupt
	
	if (Flag_INTERRUPT && Interrupts)//Has Interrupts
	{
		for (int i = 4; i >= 0; i--)// Operate 4 kinds of interrupts by order
		{
			GB_Byte tmp= InterruptEnable&InterruptFlag;
			if (GetBit(tmp,i))//decide which kind of interupt occur
			{
				//disable interrupt that has operated
				SetBit(InterruptFlag,i,false);
				memory_.WriteByte(IF_ADDRESS, InterruptFlag);
				Flag_INTERRUPT = false;
				//push now PC into stack
				memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF);
				memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF);
				//jump to interrupt operator
				Reg_PC = 0x40 + i * 0x08;
				//reset halt
				Flag_HALT = false;
				AddTime = 12;
				break;
				
				
			}
		}
	}
	else if (!Flag_INTERRUPT&&InterruptFlag&&Flag_HALT)//Operate halt
	{
		Flag_HALT = false;
		
	}
	else if (!Flag_HALT)
	{
		AddTime = GB_Opcode[memory_.ReadByte(Reg_PC++)]();//no interupts nor halt
	#ifdef DEBUG
		counts++;
		outfile << dec << counts << std::endl << "REG_PC:" << hex << Reg_PC << std::endl << " REG_A:" << hex << (int)Reg_A << " REG_F:" << hex << (int)Reg_F << " Reg_B:" << hex << (int)Reg_B << " Reg_C:" << hex << (int)Reg_C << " Reg_D:" << hex << (int)Reg_D << " Reg_E:" << hex << (int)Reg_E << " Reg_H:" << hex << (int)Reg_H << " Reg_L:" << hex << (int)Reg_L << " Reg_SP:" << hex << Reg_SP << std::endl;
		outfile << std::endl << "Flag_Z:" << GetFlag(FLAG_ZERO) << " Flag_N:" << GetFlag(FLAG_SUBTRACT) << " Flag_H:" << GetFlag(FLAG_HALFCARRY) << " Flag_C:" << GetFlag(FLAG_CARRY) << std::endl << std::endl;
	#endif

	}

	AddTime = GB_Opcode[memory_.ReadByte(Reg_PC++)]();

	return AddTime;

}



void GB_CPU::Opcode_load()
{
	//init opcodes arrays
	for (int i = 0; i < 0x100; i++)
	{
		GB_Opcode[i] = [&]()->int {return 4;};
		GB_CBOpcode[i] = [&]()->int {return 4; };
	}
	//use lamda function to describe AF,BC,DE,HL
	auto Reg_AF = [&]()-> GB_DoubleByte {return (Reg_A << 8) | Reg_F; };
	auto Reg_BC = [&]()-> GB_DoubleByte {return (Reg_B << 8) | Reg_C; };
	auto Reg_DE = [&]()-> GB_DoubleByte {return (Reg_D << 8) | Reg_E; };
	auto Reg_HL = [&]()-> GB_DoubleByte {return (Reg_H << 8) | Reg_L; };
	
	//LD nn,n
	GB_Opcode[0x06] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x0E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x16] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x1E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x26] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = memory_.ReadByte(Reg_PC++); return 8; };
	GB_Opcode[0x2E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = memory_.ReadByte(Reg_PC++); return 8; };
	
	//LD r1,r2
	GB_Opcode[0x7F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_A; return 4; };
	GB_Opcode[0x78] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_B; return 4; };
	GB_Opcode[0x79] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_C; return 4; };
	GB_Opcode[0x7A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_D; return 4; };
	GB_Opcode[0x7B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_E; return 4; };
	GB_Opcode[0x7C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_H; return 4; };
	GB_Opcode[0x7D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_L; return 4; };
	GB_Opcode[0x7E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x40] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = Reg_B; return 4; };
	GB_Opcode[0x41] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = Reg_C; return 4; };
	GB_Opcode[0x42] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = Reg_D; return 4; };
	GB_Opcode[0x43] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = Reg_E; return 4; };
	GB_Opcode[0x44] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = Reg_H; return 4; };
	GB_Opcode[0x45] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = Reg_L; return 4; };
	GB_Opcode[0x46] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B =memory_.ReadByte( Reg_HL()); return 8; };
	GB_Opcode[0x48] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = Reg_B; return 4; };
	GB_Opcode[0x49] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = Reg_C; return 4; };
	GB_Opcode[0x4A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = Reg_D; return 4; };
	GB_Opcode[0x4B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = Reg_E; return 4; };
	GB_Opcode[0x4C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = Reg_H; return 4; };
	GB_Opcode[0x4D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = Reg_L; return 4; };
	GB_Opcode[0x4E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x50] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = Reg_B; return 4; };
	GB_Opcode[0x51] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = Reg_C; return 4; };
	GB_Opcode[0x52] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = Reg_D; return 4; };
	GB_Opcode[0x53] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = Reg_E; return 4; };
	GB_Opcode[0x54] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = Reg_H; return 4; };
	GB_Opcode[0x55] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = Reg_L; return 4; };
	GB_Opcode[0x56] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x58] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = Reg_B; return 4; };
	GB_Opcode[0x59] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = Reg_C; return 4; };
	GB_Opcode[0x5A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = Reg_D; return 4; };
	GB_Opcode[0x5B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = Reg_E; return 4; };
	GB_Opcode[0x5C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = Reg_H; return 4; };
	GB_Opcode[0x5D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = Reg_L; return 4; };
	GB_Opcode[0x5E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x60] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = Reg_B; return 4; };
	GB_Opcode[0x61] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = Reg_C; return 4; };
	GB_Opcode[0x62] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = Reg_D; return 4; };
	GB_Opcode[0x63] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = Reg_E; return 4; };
	GB_Opcode[0x64] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = Reg_H; return 4; };
	GB_Opcode[0x65] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = Reg_L; return 4; };
	GB_Opcode[0x66] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0x68] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = Reg_B; return 4; };
	GB_Opcode[0x69] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = Reg_C; return 4; };
	GB_Opcode[0x6A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = Reg_D; return 4; };
	GB_Opcode[0x6B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = Reg_E; return 4; };
	GB_Opcode[0x6C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = Reg_H; return 4; };
	GB_Opcode[0x6D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = Reg_L; return 4; };
	GB_Opcode[0x6E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = memory_.ReadByte(Reg_HL()); return 4; };
	GB_Opcode[0x70] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(),Reg_B); return 8; };
	GB_Opcode[0x71] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(), Reg_C); return 8; };
	GB_Opcode[0x72] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(), Reg_D); return 8; };
	GB_Opcode[0x73] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(), Reg_E); return 8; };
	GB_Opcode[0x74] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(), Reg_H); return 8; };
	GB_Opcode[0x75] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(), Reg_L); return 8; };
	GB_Opcode[0x36] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(), memory_.ReadByte(Reg_PC++)); return 12; };
	
	//LD A,n
	GB_Opcode[0x7F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_A; return 4; };
	GB_Opcode[0x78] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_B; return 4; };
	GB_Opcode[0x79] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_C; return 4; };
	GB_Opcode[0x7A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_D; return 4; };
	GB_Opcode[0x7B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_E; return 4; };
	GB_Opcode[0x7C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_H; return 4; };
	GB_Opcode[0x7D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_L; return 4; };
	GB_Opcode[0x0A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(Reg_BC()); return 8; };
	GB_Opcode[0x1A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(Reg_DE()); return 8; };
	GB_Opcode[0x7E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(Reg_HL()); return 8; };
	GB_Opcode[0xFA] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(memory_.ReadDoubleByte(Reg_PC)); Reg_PC += 2; return 16; };
	GB_Opcode[0x3E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(Reg_PC++); return 8; };

	//LD n,A
	GB_Opcode[0x7F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = Reg_A; return 4; };
	GB_Opcode[0x47] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = Reg_A; return 4; };
	GB_Opcode[0x4F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = Reg_A; return 4; };
	GB_Opcode[0x57] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = Reg_A; return 4; };
	GB_Opcode[0x5F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = Reg_A; return 4; };
	GB_Opcode[0x67] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = Reg_A; return 4; };
	GB_Opcode[0x6F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = Reg_A; return 4; };
	GB_Opcode[0x02] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_BC(),Reg_A); return 8; };
	GB_Opcode[0x12] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_DE(), Reg_A); return 8; };
	GB_Opcode[0x77] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(Reg_HL(), Reg_A); return 8; };
	GB_Opcode[0xEA] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(memory_.ReadDoubleByte(Reg_PC), Reg_A); Reg_PC += 2; return 16; };

	//LD A,(C)
	GB_Opcode[0xF2] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(0xFF00 + Reg_C); return 8; }; 

	//LD (C),A
	GB_Opcode[0xE2] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(0xFF00 + Reg_C, Reg_A); return 8; };

	//LDD A,(HL)

	GB_Opcode[0x3A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_Opcode[0x7E](); GB_Opcode[0x2B](); return 8; };

	//LDD (HL),A
	GB_Opcode[0x32] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_Opcode[0x77](); GB_Opcode[0x2B](); return 8; };

	//LDI A,(HL)
	GB_Opcode[0x2A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_Opcode[0x7E](); GB_Opcode[0x23](); return 8; };
	
	//LDI (HL),A
	GB_Opcode[0x22] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_Opcode[0x77](); GB_Opcode[0x23](); return 8; };

	//LDH (n),A
	GB_Opcode[0xE0] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(0xFF00 + memory_.ReadByte(Reg_PC++), Reg_A); return 12; };

	//LDH A,(n)
	GB_Opcode[0xF0] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A = memory_.ReadByte(0xFF00 + memory_.ReadByte(Reg_PC++)); return 12; };

	//LD n,nn
	GB_Opcode[0x01] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = memory_.ReadByte(Reg_PC++); Reg_B = memory_.ReadByte(Reg_PC++); return 12; };
	GB_Opcode[0x11] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = memory_.ReadByte(Reg_PC++); Reg_D = memory_.ReadByte(Reg_PC++); return 12; };
	GB_Opcode[0x21] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = memory_.ReadByte(Reg_PC++); Reg_H = memory_.ReadByte(Reg_PC++); return 12; };
	GB_Opcode[0x31] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low=memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++); 
		Reg_SP = high << 8 | low;
		return 12; };


	//LD SP,HL
	GB_Opcode[0xF9] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_SP = Reg_HL(); return 8; };

	//LDHL SP,n
	GB_Opcode[0xF8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		
		GB_DoubleByte tmp= Reg_SP;

		GB_SByte byte = memory_.ReadByte(Reg_PC++);
		GB_DoubleByte result = Reg_SP + byte;
		
		SetFlag(FLAG_ZERO, false);
		SetFlag(FLAG_SUBTRACT, false);
		SetFlag(FLAG_HALFCARRY, (Reg_SP ^ byte ^ result) & 0x10);
		SetFlag(FLAG_CARRY, (Reg_SP ^ byte ^ result) & 0x100);

		Reg_SP = result;

		return 12; };

	//LD (nn),SP
	GB_Opcode[0x08] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteDoubleByte(Reg_PC, Reg_SP); Reg_PC += 2; return 20; };

	//PUSH nn
	GB_Opcode[0xF5] = [&, Reg_AF, Reg_BC, Reg_DE, Reg_HL]()->int {memory_.WriteByte(--Reg_SP, Reg_A); memory_.WriteByte(--Reg_SP, Reg_F); return 16; };
	GB_Opcode[0xC5] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, Reg_B); memory_.WriteByte(--Reg_SP, Reg_C); return 16; };
	GB_Opcode[0xD5] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, Reg_D); memory_.WriteByte(--Reg_SP, Reg_E); return 16; };
	GB_Opcode[0xE5] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, Reg_H); memory_.WriteByte(--Reg_SP, Reg_L); return 16; };
	//POP nn
	GB_Opcode[0xF1] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_F = memory_.ReadByte(Reg_SP++); Reg_A = memory_.ReadByte(Reg_SP++);  return 16; };
	GB_Opcode[0xC1] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C = memory_.ReadByte(Reg_SP++); Reg_B = memory_.ReadByte(Reg_SP++); return 16; };
	GB_Opcode[0xD1] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E = memory_.ReadByte(Reg_SP++); Reg_D = memory_.ReadByte(Reg_SP++);  return 16; };
	GB_Opcode[0xE1] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L = memory_.ReadByte(Reg_SP++); Reg_H = memory_.ReadByte(Reg_SP++); return 16; };
	//ADD A,n
	
 	GB_Opcode[0x87] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(Reg_A); return 4; };
	GB_Opcode[0x80] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(Reg_B); return 4; };
	GB_Opcode[0x81] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(Reg_C); return 4; };
	GB_Opcode[0x82] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(Reg_D); return 4; };
	GB_Opcode[0x83] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(Reg_E); return 4; };
	GB_Opcode[0x84] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(Reg_H); return 4; };
	GB_Opcode[0x85] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(Reg_L); return 4; };
	GB_Opcode[0x86] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(memory_.ReadByte(Reg_HL())) ; return 8; };
	GB_Opcode[0xC6] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADD(memory_.ReadByte(Reg_PC++)); return 8; };



	//ADC A,n
	GB_Opcode[0x8F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(Reg_A); return 4; };
	GB_Opcode[0x88] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(Reg_B); return 4; };
	GB_Opcode[0x89] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(Reg_C); return 4; };
	GB_Opcode[0x8A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(Reg_D); return 4; };
	GB_Opcode[0x8B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(Reg_E); return 4; };
	GB_Opcode[0x8C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(Reg_H); return 4; };
	GB_Opcode[0x8D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(Reg_L); return 4; };
	GB_Opcode[0x8E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xCE] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADC(memory_.ReadByte(Reg_PC++)); return 8; };


	//SUB n

	GB_Opcode[0x97] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(Reg_A); return 4; };
	GB_Opcode[0x90] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(Reg_B); return 4; };
	GB_Opcode[0x91] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(Reg_C); return 4; };
	GB_Opcode[0x92] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(Reg_D); return 4; };
	GB_Opcode[0x93] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(Reg_E); return 4; };
	GB_Opcode[0x94] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(Reg_H); return 4; };
	GB_Opcode[0x95] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(Reg_L); return 4; };
	GB_Opcode[0x96] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xD6] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SUB(memory_.ReadByte(Reg_PC++)); return 8; };



	//SBC A,n

	GB_Opcode[0x9F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(Reg_A); return 4; };
	GB_Opcode[0x98] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(Reg_B); return 4; };
	GB_Opcode[0x99] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(Reg_C); return 4; };
	GB_Opcode[0x9A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(Reg_D); return 4; };
	GB_Opcode[0x9B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(Reg_E); return 4; };
	GB_Opcode[0x9C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(Reg_H); return 4; };
	GB_Opcode[0x9D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(Reg_L); return 4; };
	GB_Opcode[0x9E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xDE] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SBC(memory_.ReadByte(Reg_PC++)); return 8; };

	//AND n

	GB_Opcode[0xA7] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(Reg_A); return 4; };
	GB_Opcode[0xA0] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(Reg_B); return 4; };
	GB_Opcode[0xA1] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(Reg_C); return 4; };
	GB_Opcode[0xA2] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(Reg_D); return 4; };
	GB_Opcode[0xA3] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(Reg_E); return 4; };
	GB_Opcode[0xA4] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(Reg_H); return 4; };
	GB_Opcode[0xA5] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(Reg_L); return 4; };
	GB_Opcode[0xA6] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xE6] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_AND(memory_.ReadByte(Reg_PC++)); return 8; };
	

	//OR n

	GB_Opcode[0xB7] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(Reg_A); return 4; };
	GB_Opcode[0xB0] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(Reg_B); return 4; };
	GB_Opcode[0xB1] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(Reg_C); return 4; };
	GB_Opcode[0xB2] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(Reg_D); return 4; };
	GB_Opcode[0xB3] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(Reg_E); return 4; };
	GB_Opcode[0xB4] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(Reg_H); return 4; };
	GB_Opcode[0xB5] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(Reg_L); return 4; };
	GB_Opcode[0xB6] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xF6] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_OR(memory_.ReadByte(Reg_PC++)); return 8; };



	//XOR n

	GB_Opcode[0xAF] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(Reg_A); return 4; };
	GB_Opcode[0xA8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(Reg_B); return 4; };
	GB_Opcode[0xA9] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(Reg_C); return 4; };
	GB_Opcode[0xAA] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(Reg_D); return 4; };
	GB_Opcode[0xAB] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(Reg_E); return 4; };
	GB_Opcode[0xAC] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(Reg_H); return 4; };
	GB_Opcode[0xAD] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(Reg_L); return 4; };
	GB_Opcode[0xAE] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xEE] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_XOR(memory_.ReadByte(Reg_PC++)); return 8; };



	//	CP n
	GB_Opcode[0xBF] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(Reg_A); return 4; };
	GB_Opcode[0xB8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(Reg_B); return 4; };
	GB_Opcode[0xB9] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(Reg_C); return 4; };
	GB_Opcode[0xBA] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(Reg_D); return 4; };
	GB_Opcode[0xBB] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(Reg_E); return 4; };
	GB_Opcode[0xBC] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(Reg_H); return 4; };
	GB_Opcode[0xBD] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(Reg_L); return 4; };
	GB_Opcode[0xBE] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(memory_.ReadByte(Reg_HL())); return 8; };
	GB_Opcode[0xFE] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_CP(memory_.ReadByte(Reg_PC++)); return 8; };


	//INC n

	GB_Opcode[0x3C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_INC(Reg_A); return 4; };
	GB_Opcode[0x04] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_INC(Reg_B); return 4; };
	GB_Opcode[0x0C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_INC(Reg_C); return 4; };
	GB_Opcode[0x14] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_INC(Reg_D); return 4; };
	GB_Opcode[0x1C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_INC(Reg_E); return 4; };
	GB_Opcode[0x24] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_INC(Reg_H); return 4; };
	GB_Opcode[0x2C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_INC(Reg_L); return 4; };
	GB_Opcode[0x34] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_INC(tmp);
		memory_.WriteByte(Reg_HL(),tmp);
		return 12; };



	//DEC n

	GB_Opcode[0x3D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_DEC(Reg_A); return 4; };
	GB_Opcode[0x05] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_DEC(Reg_B); return 4; };
	GB_Opcode[0x0D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_DEC(Reg_C); return 4; };
	GB_Opcode[0x15] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_DEC(Reg_D); return 4; };
	GB_Opcode[0x1D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_DEC(Reg_E); return 4; };
	GB_Opcode[0x25] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_DEC(Reg_H); return 4; };
	GB_Opcode[0x2D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_DEC(Reg_L); return 4; };
	GB_Opcode[0x35] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp=memory_.ReadByte(Reg_HL());
		GB_DEC(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 12; };



	//ADD HL,n

	GB_Opcode[0x09] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADDHL(Reg_BC(),Reg_HL()); return 8; };
	GB_Opcode[0x19] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADDHL(Reg_DE(),Reg_HL()); return 8; };
	GB_Opcode[0x29] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADDHL(Reg_HL(),Reg_HL()); return 8; };
	GB_Opcode[0x39] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADDHL(Reg_HL(),Reg_SP); return 8; };



	//ADD SP,n

	GB_Opcode[0xE8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_ADDSP(); return 16; };



	//INC nn

	GB_Opcode[0x03] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = ((Reg_BC() + 1) >> 8) & 0xff;Reg_C=((Reg_BC()+1))&0xFF; return 8; };
	GB_Opcode[0x13] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = ((Reg_DE() + 1) >> 8) & 0xff; Reg_E = ((Reg_DE() + 1)) & 0xFF; return 8; };
	GB_Opcode[0x23] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = ((Reg_HL() + 1) >> 8) & 0xff; Reg_L = ((Reg_HL() + 1)) & 0xFF; return 8; };
	GB_Opcode[0x33] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_SP = Reg_SP + 1; return 8; };



	//DEC nn

	GB_Opcode[0x0B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B = ((Reg_BC() - 1) >> 8) & 0xff; Reg_C = ((Reg_BC() - 1)) & 0xFF; return 8; };
	GB_Opcode[0x1B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D = ((Reg_DE() - 1) >> 8) & 0xff; Reg_E = ((Reg_DE() - 1)) & 0xFF; return 8; };
	GB_Opcode[0x2B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H = ((Reg_HL() - 1) >> 8) & 0xff; Reg_L = ((Reg_HL() - 1)) & 0xFF; return 8; };
	GB_Opcode[0x3B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_SP = Reg_SP - 1;; return 8; };
	GB_Opcode[0xCB] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {return GB_CBOpcode[memory_.ReadByte(Reg_PC++)](); };

	//SWAP n

	GB_CBOpcode[0x37] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SWAP(Reg_A); return 8; };
	GB_CBOpcode[0x30] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SWAP(Reg_B); return 8; };
	GB_CBOpcode[0x31] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SWAP(Reg_C); return 8; };
	GB_CBOpcode[0x32] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SWAP(Reg_D); return 8; };
	GB_CBOpcode[0x33] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SWAP(Reg_E); return 8; };
	GB_CBOpcode[0x34] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SWAP(Reg_H); return 8; };
	GB_CBOpcode[0x35] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SWAP(Reg_L); return 8; };
	GB_CBOpcode[0x36] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_SWAP(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16;
	};


	//DAA
	GB_Opcode[0x27] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = Reg_A;
		if (GetFlag(FLAG_SUBTRACT))
		{
			if(GetFlag(FLAG_CARRY))
			{tmp = (tmp-0x60)&0xFF;}
			if (GetFlag(FLAG_HALFCARRY))
			{tmp = (tmp - 0x06) & 0xFF;}
		}
		else
		{
			if ((tmp & 0x0F) > 0x09 || GetFlag(FLAG_HALFCARRY))
			{tmp += 0x06;}
			if (tmp > 0x9F || GetFlag(FLAG_CARRY))
			{tmp += 0x60;}
		}

		SetFlag(FLAG_ZERO, (tmp == 0));
		SetFlag(FLAG_HALFCARRY, false);
		SetFlag(FLAG_CARRY, (tmp & 0x100));
		return 4; };

	//CPL
	GB_Opcode[0x2F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_SUBTRACT,true); SetFlag(FLAG_HALFCARRY,true); Reg_A = ~Reg_A; return 4; };

	//CCF
	GB_Opcode[0x3F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); SetFlag(FLAG_CARRY,!GetFlag(FLAG_CARRY)); return 4; };

	//SCF
	GB_Opcode[0x37] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); SetFlag(FLAG_HALFCARRY,true); return 4; };

	//NOP
	GB_Opcode[0x00] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {return 4; };

	//HALT
	GB_Opcode[0x76] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Flag_HALT = true; return 4; };

	//STOP
	GB_Opcode[0x10] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Flag_HALT = true; return 4; };

	//DI
	GB_Opcode[0xF3] = [&, Reg_AF, Reg_BC, Reg_DE, Reg_HL]() -> int {Flag_INTERRUPT = false;  return 4; };

	//EI
	GB_Opcode[0xFB] = [&, Reg_AF, Reg_BC, Reg_DE, Reg_HL]() ->int {Flag_INTERRUPT = true; return 4; };

	//RLCA
	GB_Opcode[0x07] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_A); return 4; };

	//RLA
	GB_Opcode[0x17] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_A); return 4; };

	//RRCA
	GB_Opcode[0x0F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_A); return 4; };

	//RRA
	GB_Opcode[0x1F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_A); return 4; };

	//RLC
	GB_CBOpcode[0x07] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_A); return 8; };
	GB_CBOpcode[0x00] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_B); return 8; };
	GB_CBOpcode[0x01] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_C); return 8; };
	GB_CBOpcode[0x02] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_D); return 8; };
	GB_CBOpcode[0x03] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_E); return 8; };
	GB_CBOpcode[0x04] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_H); return 8; };
	GB_CBOpcode[0x05] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RLC(Reg_L); return 8; };
	GB_CBOpcode[0x06] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_RLC(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };

	//RL
	GB_CBOpcode[0x17] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_A); return 8; };
	GB_CBOpcode[0x10] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_B); return 8; };
	GB_CBOpcode[0x11] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_C); return 8; };
	GB_CBOpcode[0x12] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_D); return 8; };
	GB_CBOpcode[0x13] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_E); return 8; };
	GB_CBOpcode[0x14] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_H); return 8; };
	GB_CBOpcode[0x15] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RL(Reg_L); return 8; };
	GB_CBOpcode[0x16] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_RL(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };

	//RRC
	GB_CBOpcode[0x0F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_A); return 8; };
	GB_CBOpcode[0x08] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_B); return 8; };
	GB_CBOpcode[0x09] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_C); return 8; };
	GB_CBOpcode[0x0A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_D); return 8; };
	GB_CBOpcode[0x0B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_E); return 8; };
	GB_CBOpcode[0x0C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_H); return 8; };
	GB_CBOpcode[0x0D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_L); return 8; };
	GB_CBOpcode[0x0E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_RRC(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };

	//RRC
	GB_CBOpcode[0x0F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_A); return 8; };
	GB_CBOpcode[0x08] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_B); return 8; };
	GB_CBOpcode[0x09] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_C); return 8; };
	GB_CBOpcode[0x0A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_D); return 8; };
	GB_CBOpcode[0x0B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_E); return 8; };
	GB_CBOpcode[0x0C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_H); return 8; };
	GB_CBOpcode[0x0D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RRC(Reg_L); return 8; };
	GB_CBOpcode[0x0E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_RRC(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };
	
	//RR
	GB_CBOpcode[0x1F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_A); return 8; };
	GB_CBOpcode[0x18] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_B); return 8; };
	GB_CBOpcode[0x19] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_C); return 8; };
	GB_CBOpcode[0x1A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_D); return 8; };
	GB_CBOpcode[0x1B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_E); return 8; };
	GB_CBOpcode[0x1C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_H); return 8; };
	GB_CBOpcode[0x1D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_RR(Reg_L); return 8; };
	GB_CBOpcode[0x1E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_RR(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };

	//SLA
	GB_CBOpcode[0x27] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SLA(Reg_A); return 8; };
	GB_CBOpcode[0x20] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SLA(Reg_B); return 8; };
	GB_CBOpcode[0x21] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SLA(Reg_C); return 8; };
	GB_CBOpcode[0x22] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SLA(Reg_D); return 8; };
	GB_CBOpcode[0x23] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SLA(Reg_E); return 8; };
	GB_CBOpcode[0x24] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SLA(Reg_H); return 8; };
	GB_CBOpcode[0x25] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SLA(Reg_L); return 8; };
	GB_CBOpcode[0x26] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_SLA(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };

	//SRA
	GB_CBOpcode[0x2F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRA(Reg_A); return 8; };
	GB_CBOpcode[0x28] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRA(Reg_B); return 8; };
	GB_CBOpcode[0x29] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRA(Reg_C); return 8; };
	GB_CBOpcode[0x2A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRA(Reg_D); return 8; };
	GB_CBOpcode[0x2B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRA(Reg_E); return 8; };
	GB_CBOpcode[0x2C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRA(Reg_H); return 8; };
	GB_CBOpcode[0x2D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRA(Reg_L); return 8; };
	GB_CBOpcode[0x2E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_SRA(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };

	//SRL
	GB_CBOpcode[0x3F] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRL(Reg_A); return 8; };
	GB_CBOpcode[0x38] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRL(Reg_B); return 8; };
	GB_CBOpcode[0x39] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRL(Reg_C); return 8; };
	GB_CBOpcode[0x3A] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRL(Reg_D); return 8; };
	GB_CBOpcode[0x3B] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRL(Reg_E); return 8; };
	GB_CBOpcode[0x3C] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRL(Reg_H); return 8; };
	GB_CBOpcode[0x3D] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_SRL(Reg_L); return 8; };
	GB_CBOpcode[0x3E] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte tmp = memory_.ReadByte(Reg_HL());
		GB_SRL(tmp);
		memory_.WriteByte(Reg_HL(), tmp);
		return 16; };
	//BIT
	for (int i = 0; i < 8; i++) {
		GB_CBOpcode[0x40 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_ZERO,!((Reg_B >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 8; };
		GB_CBOpcode[0x41 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_ZERO, !((Reg_C >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 8; };
		GB_CBOpcode[0x42 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_ZERO, !((Reg_D >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 8; };
		GB_CBOpcode[0x43 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_ZERO, !((Reg_E >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 8; };
		GB_CBOpcode[0x44 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_ZERO, !((Reg_H >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 8; };
		GB_CBOpcode[0x45 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_ZERO, !((Reg_L >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 8; };
		GB_CBOpcode[0x46 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_Byte tmp = memory_.ReadByte(Reg_HL()); SetFlag(FLAG_ZERO, !((tmp >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 16; };
		GB_CBOpcode[0x47 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {SetFlag(FLAG_ZERO, !((Reg_A >> i) & 0x01)); SetFlag(FLAG_SUBTRACT,false); SetFlag(FLAG_HALFCARRY,false); return 8; };
	}
	//SET
	for (int i = 0; i < 8; i++) {
		GB_CBOpcode[0xC0 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B |= 1 << i; return 8; };
		GB_CBOpcode[0xC1 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C |= 1 << i; return 8; };
		GB_CBOpcode[0xC2 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D |= 1 << i; return 8; };
		GB_CBOpcode[0xC3 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E |= 1 << i; return 8; };
		GB_CBOpcode[0xC4 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H |= 1 << i; return 8; };
		GB_CBOpcode[0xC5 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L |= 1 << i; return 8; };
		GB_CBOpcode[0xC6 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_Byte tmp = memory_.ReadByte(Reg_HL()); tmp |= 1 << i; memory_.WriteByte(Reg_HL(), tmp); return 16; };
		GB_CBOpcode[0xC7 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A |= 1 << i; return 8; };
	}
	//RES
	for (int i = 0; i < 8; i++) {
		GB_CBOpcode[0x80 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_B ^= (0 ^ Reg_B) & (1 << i); return 8; };
		GB_CBOpcode[0x81 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_C ^= (0 ^ Reg_C) & (1 << i); return 8; };
		GB_CBOpcode[0x82 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_D ^= (0 ^ Reg_D) & (1 << i); return 8; };
		GB_CBOpcode[0x83 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_E ^= (0 ^ Reg_E) & (1 << i); return 8; };
		GB_CBOpcode[0x84 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_H ^= (0 ^ Reg_H) & (1 << i); return 8; };
		GB_CBOpcode[0x85 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_L ^= (0 ^ Reg_L) & (1 << i);; return 8; };
		GB_CBOpcode[0x86 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {GB_Byte tmp = memory_.ReadByte(Reg_HL()); tmp ^= (0 ^ tmp) & (1 << i); memory_.WriteByte(Reg_HL(), tmp); return 16; };
		GB_CBOpcode[0x87 + i * 8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {Reg_A ^= (0 ^ Reg_A) & (1 << i); return 8; };
	}

	//JP
	GB_Opcode[0xC3] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		Reg_PC = high << 8 | low;
		return 12;
	};

	GB_Opcode[0xC2] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_ZERO) == false) {
			Reg_PC = high << 8 | low;
		}
		return 12;
	};
	
	GB_Opcode[0xCA] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_ZERO) == true) {
			Reg_PC = high << 8 | low;
		}
		return 12;
	};
	
	GB_Opcode[0xD2] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_CARRY) == false) {
			Reg_PC = high << 8 | low;
		}
		return 12;
	};

	GB_Opcode[0xDA] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_CARRY) == true) {
			Reg_PC = high << 8 | low;
		}
		return 12;
	};

	GB_Opcode[0xE9] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		Reg_PC = Reg_HL();
		return 4;
	};
		
	GB_Opcode[0x18] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		Reg_PC += (GB_SByte)memory_.ReadByte(Reg_PC++);
		return 8;
	};

	GB_Opcode[0x20] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_SByte tmp= memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_ZERO) == false) {
			Reg_PC += tmp;
		}
		return 8;
	};
	
	GB_Opcode[0x28] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_SByte tmp = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_ZERO) == true) {
			Reg_PC += tmp;
		}
		return 8;
	};

	GB_Opcode[0x30] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_SByte tmp = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_CARRY) == false) {
			Reg_PC += tmp;
		}
		return 8;
	};

	GB_Opcode[0x38] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_SByte tmp = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_CARRY) == true) {
			Reg_PC += tmp;
		}
		return 8;
	};

	//Calls

	GB_Opcode[0xCD] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF);
		memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF);
		Reg_PC = high << 8 | low;
		return 12;
	};

	GB_Opcode[0xC4] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_ZERO) == false) {
			memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF);
			memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF);
			Reg_PC = high << 8 | low;
		}
		return 12;
	};

	GB_Opcode[0xCC] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_ZERO) == true) {
			memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF);
			memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF);
			Reg_PC = high << 8 | low;
		}
		return 12;
	};

	GB_Opcode[0xD4] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_CARRY) == false) {
			memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF);
			memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF);
			Reg_PC = high << 8 | low;
		}
		return 12;
	};

	GB_Opcode[0xDC] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_PC++);
		GB_Byte high = memory_.ReadByte(Reg_PC++);
		if (GetFlag(FLAG_CARRY) == true) {
			memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF);
			memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF);
			Reg_PC = high << 8 | low;
		}
		return 12;
	};

	//RST
	GB_Opcode[0xC7] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x00; return 32;};
	GB_Opcode[0xCF] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x08; return 32; };
	GB_Opcode[0xD7] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x10; return 32; };
	GB_Opcode[0xDF] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x18; return 32; };
	GB_Opcode[0xE7] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x20; return 32; };
	GB_Opcode[0xEF] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x28; return 32; };
	GB_Opcode[0xF7] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x30; return 32; };
	GB_Opcode[0xFF] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {memory_.WriteByte(--Reg_SP, (Reg_PC >> 8) & 0xFF); memory_.WriteByte(--Reg_SP, Reg_PC & 0xFF); Reg_PC = 0x38; return 32; };

	//RET
	GB_Opcode[0xC9] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_SP++);
		GB_Byte high = memory_.ReadByte(Reg_SP++);
		Reg_PC = high << 8 | low;
		return 8;
	};

	GB_Opcode[0xC0] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
	
		if (GetFlag(FLAG_ZERO) == false) {
			GB_Byte low = memory_.ReadByte(Reg_SP++);
			GB_Byte high = memory_.ReadByte(Reg_SP++);
			Reg_PC = high << 8 | low;
		}
		return 8;
	};
	
	GB_Opcode[0xC8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		
		if (GetFlag(FLAG_ZERO) == true) {
			GB_Byte low = memory_.ReadByte(Reg_SP++);
			GB_Byte high = memory_.ReadByte(Reg_SP++);
			Reg_PC = high << 8 | low;
		}
		return 8;
	};

	GB_Opcode[0xD0] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		
		if (GetFlag(FLAG_CARRY) == false) {
			GB_Byte low = memory_.ReadByte(Reg_SP++);
			GB_Byte high = memory_.ReadByte(Reg_SP++);
			Reg_PC = high << 8 | low;
		}
		return 8;
	};

	GB_Opcode[0xD8] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		
		if (GetFlag(FLAG_CARRY) == true) {
			GB_Byte low = memory_.ReadByte(Reg_SP++);
			GB_Byte high = memory_.ReadByte(Reg_SP++);
			Reg_PC = high << 8 | low;
		}
		return 8;
	};

	//RETI
	GB_Opcode[0xD9] = [&,Reg_AF,Reg_BC,Reg_DE,Reg_HL]()->int {
		GB_Byte low = memory_.ReadByte(Reg_SP++);
		GB_Byte high = memory_.ReadByte(Reg_SP++);
		Reg_PC = high << 8 | low;
		Flag_INTERRUPT = true;
		return 8;
	};
	






}

void GB_CPU::GB_ADD(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A + num1;
	
	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT,0);
	SetFlag(FLAG_HALFCARRY,(tmp^Reg_A^num1) & 0x10);
	SetFlag(FLAG_CARRY, (tmp^Reg_A^num1) & 0x100);
	
	Reg_A = tmp&0xFF;
}

void GB_CPU::GB_ADC(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A + num1 +GetFlag(FLAG_CARRY);

	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT,0);
	SetFlag(FLAG_HALFCARRY,(tmp^Reg_A^num1) & 0x10);
	SetFlag(FLAG_CARRY,(tmp^Reg_A^num1) & 0x100);

	Reg_A = tmp & 0xFF;
}

void GB_CPU::GB_SUB(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A - num1;

	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT, 1);
	SetFlag(FLAG_HALFCARRY,(tmp^Reg_A^num1) & 0x10);
	SetFlag(FLAG_CARRY,(tmp^Reg_A^num1) & 0x100);

	Reg_A = tmp & 0xFF;
}

void GB_CPU::GB_SBC(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A - num1 - GetFlag(FLAG_CARRY);

	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT, 1);
	SetFlag(FLAG_HALFCARRY,(tmp^Reg_A^num1) & 0x10);
	SetFlag(FLAG_CARRY,(tmp^Reg_A^num1) & 0x100);

	Reg_A = tmp & 0xFF;
}

void GB_CPU::GB_AND(GB_Byte num1)
{
	Reg_A &= num1;

	SetFlag(FLAG_ZERO,Reg_A==0);
	SetFlag(FLAG_SUBTRACT,0);
	SetFlag(FLAG_HALFCARRY, 1);
	SetFlag(FLAG_CARRY,0);

	
}

void GB_CPU::GB_OR(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A | num1;

	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT,0);
	SetFlag(FLAG_HALFCARRY,0);
	SetFlag(FLAG_CARRY,0);

	Reg_A = tmp&0xFF;
}

void GB_CPU::GB_XOR(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A ^ num1;

	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT,0);
	SetFlag(FLAG_HALFCARRY,0);
	SetFlag(FLAG_CARRY,0);

	Reg_A = tmp&0xFF;
}

void GB_CPU::GB_CP(GB_Byte num1)
{
	GB_DoubleByte tmp = Reg_A - num1;
	
	SetFlag(FLAG_ZERO, (tmp == 0 || tmp==0x100));
	SetFlag(FLAG_SUBTRACT, 1);
	SetFlag(FLAG_HALFCARRY,(tmp^Reg_A^num1) & 0x10);
	SetFlag(FLAG_CARRY,((tmp^Reg_A^num1) & 0x100) );


}

void GB_CPU::GB_INC(GB_Byte &num1)
{
	GB_DoubleByte tmp = num1 + 1;

	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT,0);
	SetFlag(FLAG_HALFCARRY,(tmp^Reg_A^num1) & 0x10);

	num1 = tmp&0xFF;

}

void GB_CPU::GB_DEC(GB_Byte &num1)
{
	GB_DoubleByte tmp = num1 - 1;

	SetFlag(FLAG_ZERO,(tmp == 0 || tmp == 0x100));
	SetFlag(FLAG_SUBTRACT, 1);
	SetFlag(FLAG_HALFCARRY,(tmp^Reg_A^num1) & 0x10);

	num1 = tmp & 0xFF;

}

void GB_CPU::GB_ADDHL(GB_DoubleByte num1,GB_DoubleByte num2)
{
	int tmp = num1 + num2;

	SetFlag(FLAG_SUBTRACT,0);
	SetFlag(FLAG_CARRY, (num1^num2^tmp) & 0x10000);
	tmp &= 0xffff;
	SetFlag(FLAG_HALFCARRY, (num1^num2^tmp) & 0x1000);
	

	Reg_H = (tmp >> 8) & 0xFF;
	Reg_L = tmp & 0xFF;



	
	
}

void GB_CPU::GB_ADDSP()
{
	GB_SByte byte= (GB_SByte) memory_.ReadByte(Reg_PC++);
	GB_DoubleByte tmp = Reg_SP + byte;

	
	SetFlag(FLAG_ZERO, false);
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY, (byte^tmp^Reg_SP) & 0x10);
	SetFlag(FLAG_CARRY, (byte^tmp^Reg_SP) & 0x100);
	
	Reg_SP = tmp;
}

void GB_CPU::GB_SWAP(GB_Byte &num1)
{
	GB_Byte temp = num1 & 0x0f;
	num1 >>= 4;
	num1 |= (temp << 4);
}

void GB_CPU::GB_RLC(GB_Byte &num1)
{
	SetFlag(FLAG_CARRY, (num1 & 0x80) >> 7);
	num1 <<= 1;
	num1 += GetFlag(FLAG_CARRY);

	SetFlag(FLAG_ZERO, (num1 == 0));
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY,false);

}

void GB_CPU::GB_RL(GB_Byte &num1)
{
	bool tmp =GetFlag( FLAG_CARRY);
	SetFlag(FLAG_CARRY,(num1 & 0x80) >> 7);
	num1 <<= 1;
	num1 += tmp;

	SetFlag(FLAG_ZERO,(num1 == 0));
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY,false);

}

void GB_CPU::GB_RRC(GB_Byte &num1)
{
	SetFlag(FLAG_CARRY, num1 & 0x01);
	GB_DoubleByte tmp =GetFlag( FLAG_CARRY) << 7;
	num1 >>= 1;
	num1 += tmp;

	SetFlag(FLAG_ZERO, (num1 == 0));
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY,false);
}

void GB_CPU::GB_RR(GB_Byte &num1)
{
	GB_DoubleByte tmp = GetFlag(FLAG_CARRY) << 7;
	SetFlag(FLAG_CARRY, num1 & 0x01);
	num1 >>= 1;
	num1 += tmp;

	SetFlag(FLAG_ZERO,(num1 == 0));
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY,false);
}

void GB_CPU::GB_SLA(GB_Byte &num1)
{
	SetFlag(FLAG_CARRY ,num1 & 0x80);
	num1 <<= 1;

	SetFlag(FLAG_ZERO,(num1 == 0));
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY,false);
}

void GB_CPU::GB_SRA(GB_Byte &num1)
{
	GB_Byte tmp = num1 & 0x80;
	SetFlag(FLAG_CARRY, num1 & 0x01);
	num1 >>= 1;
	num1 |= tmp;

	SetFlag(FLAG_ZERO,(num1 == 0));
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY,false);
}

void GB_CPU::GB_SRL(GB_Byte &num1)
{
	SetFlag(FLAG_CARRY,num1 & 0x01);
	num1 >>= 1;

	SetFlag(FLAG_ZERO,(num1 == 0));
	SetFlag(FLAG_SUBTRACT,false);
	SetFlag(FLAG_HALFCARRY,false);
}
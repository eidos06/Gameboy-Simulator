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




}

void GB_CPU::CPU_Step() {

}
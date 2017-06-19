#pragma once
#include <vector>
#include <stdint.h>
#include "GB_Const.h"
using namespace std;
class GB_memory
{
public:
	void LoadMemory(const vector<GB_Byte> &inRom);

	void init();

	GB_Byte ReadByte(GB_DoubleByte Address);
	bool WriteByte(GB_DoubleByte Address, GB_Byte Value);
	bool WriteDoubleByte(GB_DoubleByte Address, GB_DoubleByte Value);
	GB_DoubleByte ReadDoubleByte(GB_DoubleByte Address);
	
	string Get_Title();
	int Get_Cartridge_Type() { return GB_Rom[0x147]; }
	int Get_ROM_Size() { return GB_Rom[0x148]; }
	int Get_RAM_Size(){return GB_Rom[0x149]; }

	GB_Byte KeysC0 = 0x0F;
	GB_Byte KeysC1 = 0x0F;
	GB_Byte KeyColumn = 0x00;
private:
	vector<GB_Byte> GB_Rom;
	vector<GB_Byte> GB_Ram;
	

};



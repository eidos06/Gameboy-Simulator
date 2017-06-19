#include "GB_memory.h"
#include <iostream>
using namespace std;
void GB_memory::LoadMemory(const vector<GB_Byte> &inRom)
{
	GB_Rom = inRom;
	GB_Ram.assign(0xFFFF - 0x8000+1, 0x00);//init RAM
}

GB_Byte GB_memory::ReadByte(GB_DoubleByte Address)
{
	
	if (Address < 0x8000)
		return GB_Rom[Address];
	else if (Address >= 0xE000 && Address <= 0xFDFF)//E000 is an echo of C000
		return GB_Ram[0xC000 + (Address - 0xE000) - 0x8000];
	else if (Address == 0xFF00)//get input
	{
		if (KeyColumn == 0x10)
		{
			
			//cout << "0x10: " << (int)KeysC0 << endl;
			return KeysC0;
		}
			
		if (KeyColumn == 0x20)
		{
			//cout << "0x20: " << (int)KeysC1 << endl;
			return KeysC1;
		}
		return 0;
	}
	else {
		return GB_Ram[Address - 0x8000];
	}
		
}

bool GB_memory::WriteByte(GB_DoubleByte Address, GB_Byte Value)
{
	if (Address < 0x8000)
		GB_Rom[Address] = Value;
	else if (Address >= 0xC000 && Address <= 0xDDFF)//E000 is an echo of C000
	{
		if (Address >= 0xC000 && Address < 0xC0A0)
			GB_Ram[0xFE00 + (Address - 0xC000) - 0x8000] = Value;
		GB_Ram[Address-0x8000] = Value;
		GB_Ram[0xE000 + (Address - 0xC000) - 0x8000] = Value;
	}
	else if (Address == 0xFF00)//input
	{
		KeyColumn = Value & 0x30;
	}

	else
	{
		GB_Ram[Address - 0x8000] = Value;
	}
	
	if (Address >= 0xE000 && Address <= 0xFDFF)//E000 is an echo of C000
	{
		GB_Ram[(Address - 0xE000)+0xC000-0x8000] = Value;
	}
	return true;
}

string GB_memory::Get_Title()
{
	string sTitle;

	for (int i = 0x0134; i <= 0x0142; i++)
	{
		if (GB_Rom[i] != 0x00)
		{
			sTitle += GB_Rom[i];
		}
	}

	return sTitle;
}

void GB_memory::init()
{
	WriteByte(TIMA_ADDRESS, 0x00);
	WriteByte(TMA_ADDRESS, 0x00);
	WriteByte(TAC_ADDRESS, 0x00);
	WriteByte(NR10_ADDRESS, 0x80);
	WriteByte(NR11_ADDRESS, 0xBF);
	WriteByte(NR12_ADDRESS, 0xF3);
	WriteByte(NR14_ADDRESS, 0xBF);
	WriteByte(NR21_ADDRESS, 0x3F);
	WriteByte(NR22_ADDRESS, 0x00);
	WriteByte(NR24_ADDRESS, 0xBF);
	WriteByte(NR30_ADDRESS, 0x7F);
	WriteByte(NR31_ADDRESS, 0xFF);
	WriteByte(NR32_ADDRESS, 0x9F);
	WriteByte(NR33_ADDRESS, 0xBF);
	WriteByte(NR41_ADDRESS, 0xFF);
	WriteByte(NR42_ADDRESS, 0x00);
	WriteByte(NR43_ADDRESS, 0x00);
	WriteByte(NR44_ADDRESS, 0xBF);
	WriteByte(NR50_ADDRESS, 0x77);
	WriteByte(NR51_ADDRESS, 0xF3);
	WriteByte(NR52_ADDRESS, 0xF1);
	WriteByte(LCDC_ADDRESS, 0x91);
	WriteByte(SCY_ADDRESS, 0x00);
	WriteByte(SCX_ADDRESS, 0x00);
	WriteByte(LYC_ADDRESS, 0x00);
	WriteByte(BGP_ADDRESS, 0xFC);
	WriteByte(OBP0_ADDRESS, 0xFF);
	WriteByte(OBP1_ADDRESS, 0xFF);
	WriteByte(WY_ADDRESS, 0x00);
	WriteByte(WX_ADDRESS, 0x00);
	WriteByte(IE_ADDRESS, 0x00);


}

GB_DoubleByte GB_memory::ReadDoubleByte(GB_DoubleByte Address)
{
	
	return ReadByte(Address+1) << 8 | ReadByte(Address);
}

bool GB_memory::WriteDoubleByte(GB_DoubleByte Address, GB_DoubleByte Value)
{
	WriteByte(Address, Value & 0x00FF);
	WriteByte(Address + 1,( Value & 0xFF00)>>8);
	return true;
}


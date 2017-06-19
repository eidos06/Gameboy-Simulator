#include "GB_Bits.h"

void SetBit(GB_Byte &Byte, int bit, bool value)
{
	Byte ^= (-value ^ Byte) & (1 << bit);
}

bool GetBit(GB_Byte &Byte, int pos)
{
	return (Byte >> pos) & 1;
}
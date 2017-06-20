#include "GB_memory.h"
#include "GB_Bits.h"
#include "GB_Const.h"
#include "GB_CPU.h"
#include "GB_Timer.h"

int GB_TIME::increase(int timing)
	{
		_clock_sub += timing;

		if (_clock_sub >= 4)
		{
			_clock_main++;
			_clock_sub -= 4;

			_clock_div++;
			if (_clock_div == 16)
			{
				_reg_div = (_reg_div + 1) & 255;
				_clock_div = 0;
			}
		}
		if (check())
		{
			return rb(0xFF06);
			//Call CPU interrupt!!!
		}
		return NULL;
	}
	bool GB_TIME::check()
	{
		if (_reg_tac % 4)
		{
			switch (_reg_tac % 3)
			{
			case 0: threshold = 64; break;		// 4K
			case 1: threshold = 1; break;		// 256K
			case 2: threshold = 4; break;		// 64K
			case 3: threshold = 16; break;		// 16K
			}

			if (_clock_main >= threshold)
				if (step())
					return true;
		}
	}

	bool GB_TIME::step()
	{
		_clock_main = 0;
		_reg_tima++;

		if (_reg_tima > 255)
		{
			_reg_tima = _reg_tma;

			GB_Byte add = memory_.ReadByte(0xFF0F);
			SetBit(add, 2, true);
			memory_.WriteByte(0xFF0F, add);
				return true;
		}
		return false;
	}

	int GB_TIME::rb(int addr)
	{
		switch (addr)
		{
		case 0xFF04: return _reg_div;
		case 0xFF05: return _reg_tima;
		case 0xFF06: return _reg_tma;
		case 0xFF07: return _reg_tac;
		}
	}

	void GB_TIME::wb(int addr, int val)
	{
		switch (addr)
		{
		case 0xFF04: _reg_div = 0; break;
		case 0xFF05: _reg_tima = val; break;
		case 0xFF06: _reg_tma = val; break;
		case 0xFF07: _reg_tac = val & 7; break;
		}
	}


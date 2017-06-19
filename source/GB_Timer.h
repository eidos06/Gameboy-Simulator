#pragma once
class GB_TIME
{
public:
	int increase(int timing);
	
private:
	int _clock_main = 0;
	int _clock_sub = 0;
	int _clock_div = 0;

	int _reg_div = 0;
	int _reg_tima = 0;
	int _reg_tma = 0;
	int _reg_tac = 0;

	int threshold = 0;

	bool check();
	bool step();
	int rb(int addr);
	void wb(int addr, int val);
	

};
#pragma once
#include <string>
#include <fstream>
#include "GB_memory.h"

using namespace std;

class GB_FileOperator
{
public:
	 GB_FileOperator();
	 int OpenFile(string sfile);
	 vector<GB_Byte> GetData();
private:
	string sFileName;
	vector<GB_Byte> ROM;
};
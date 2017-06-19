#include "GB_FileOperator.h"
using namespace std;

GB_FileOperator::GB_FileOperator()
{	//init the File Operator
	ROM.assign(0x7fff, 0);
	sFileName = "";
}

int GB_FileOperator::OpenFile(string sFile)
{
	//read file in binary mode
	sFileName = sFile;
	ifstream fRomInput{ sFileName, ios::binary };
	if (!fRomInput.is_open())
	{
		return 1;
	}
	else
	{
		//put file into ROM
		ROM.assign(istreambuf_iterator<char>(fRomInput), istreambuf_iterator<char>());
		return 0;
	}
}

vector<GB_Byte> GB_FileOperator::GetData()
{
	return ROM;
}
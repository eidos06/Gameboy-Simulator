#include"main.h"
using namespace std;

int main(int argc, char *argv[])
{
	cout << "============Welcome============" << endl;
	cout << "Welcome to our Game Boy Emulator!" << endl;
	cout << "This emulator is for final project and it still has some bugs to fix" << endl;
	cout << "Enjoy your self!!!" << endl;
	cout << "============Keys===============" << endl;
	cout << "UP/DOWN/RIGHT/LEFT" << endl;
	cout << "Z--A  X--B" << endl;
	cout << "SPACE--SELECT  ENTER--ENTER" << endl;
	cout << endl;
	//Read the GB File
	GB_FileOperator file;
	string sFileName;
	if (argc > 1)//get file by command
	{
		sFileName = string(argv[1]);
	}
	else
	{
		cout << "Please input the file name" << endl;
		cin >> sFileName;
	}

	if (file.OpenFile(sFileName))
	{
		cout << "Open File Error!" << endl;
		return 1;
	}
	//Load in memory
	memory_.LoadMemory(file.GetData());
	//Print GB info if DEBUG mode
	#ifdef DEBUG
	PrintInfo(memory_);
	#endif // DEBUG
	memory_.init();
	//create window
	MainWindow.create(160, 144, 100, 100, memory_.Get_Title());
	//init CPU
	CPU.init();
	GB_TIME GB_Timer;
	//CPU Cycle
	while (1) {
		if (MainWindow.fresh())//get KEYS
		{
			int timing = CPU.CPU_Step();//get TIMING
			MainWindow.AddTime(timing);//fresh WINDOW
			GB_Timer.increase(timing);//fresh TIMER
		}
		else
			break;//quit
	}
	#ifdef DEBUG//Write out RAM if in DEBUG mode
	ofstream outRAM;
	outRAM.open("outRAM.gb", ios::binary);

	for (int i = 0x0000; i <= 0xFFFF; i++)
	{
		outRAM << memory_.ReadByte(i);
	}
	outRAM.close();
	cout << "Writen" << endl;
	#endif
	MainWindow.end();//exit window and clear buffer
	return 0;
}

void PrintInfo(GB_memory &GB_ROM)
{
	string Cartridge_Type[] = {"ROM ONLY","ROM+MBC1","ROM+MBC1+RAM"};
	string ROM_Size[] = { "32KB","64KB","128KB","256KB","512KB" };
	string RAM_Size[] = { "None","2KB","32KB","128KB" };
	cout << "TITLE: " << GB_ROM.Get_Title() << endl;
	cout << "Cartridge Type: " << Cartridge_Type[GB_ROM.Get_Cartridge_Type()] << endl;
	cout << "ROM Size: " << ROM_Size[GB_ROM.Get_ROM_Size()] << endl;
	cout << "RAM Size: " << RAM_Size[GB_ROM.Get_RAM_Size()] << endl;
}
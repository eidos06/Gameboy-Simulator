#include"main.h"


using namespace std;

int
main(int argc, char *argv[])
{
	//Read the GB File//
	GB_FileOperator file;

	string sFileName;
	cout << "Please input the file name" << endl;
	cin >> sFileName;

	if (file.OpenFile(sFileName))
	{
		cout << "Open File Error!" << endl;
		return 1;
	}
	//Load in memory//
	memory_.LoadMemory(file.GetData());
	PrintInfo(memory_);
	memory_.init();
	//Window init
	MainWindow.create(256, 256, 100, 100, memory_.Get_Title());
	
	CPU.init();
	//CPU Cycle
	//while (1) {
	
		CPU.CPU_Step();
		MainWindow.reDraw();
		//if (!MainWindow.fresh())
	//	{
	//		break;
	//	}
		
	//}

	while (1) {
		if (!MainWindow.fresh())
		{
			break;
		}

	}
	MainWindow.end();
	
	

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
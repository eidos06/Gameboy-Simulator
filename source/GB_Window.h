#pragma once
#include <SDL.h>
#include <string>
#include "GB_memory.h"
#include "GB_Type.h"
using namespace std;
extern GB_memory memory_;

class GB_Window {
public:
	class SpriteInfo {
	public:
		GB_Byte y{ 0 };
		GB_Byte x{ 0 };
		GB_Byte tile{ 0 };
		GB_Byte flags{ 0 };

		SpriteInfo(int id);
	};

	void create (int WINDOW_WIDTH, int WINDOW_HEIGHT, int x, int y, string WINDOW_TITLE);
	void AddTime(int clocks);
	void setPixel(int x, int y, int color);
	int fresh();
	void draw();
	void end();
	void drawLine(int ly);
	vector<SpriteInfo> getSprites(int ly);
	void resetInterruptFlags();
	void SetMode(int mode);
	void updateLyc();

	int NowLine = 0;
	int NowMode = 0;
	int mode = 0;
	int counts = 0;
private:
	SDL_Window *MainWindow;
	SDL_Surface *MainSurface;
	SDL_Event e;

	int width;
	int height;
	
	int done{ 0 };
	int InnerClock=0;
};

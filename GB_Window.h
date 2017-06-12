#pragma once
#include <SDL.h>
#include <string>
#include "GB_memory.h"
#include "GB_Type.h"
using namespace std;
extern GB_memory memory_;


class GB_Window {
public:
	
	void create (int WINDOW_WIDTH, int WINDOW_HEIGHT, int x, int y, string WINDOW_TITLE);
	int fresh();
	void AddTime(int clocks);
	void end();
	int NowLine = 0;
	int NowMode = 0;
	void setPixel(int x, int y, int color);
	void draw();
	int mode = 0;
	int counts = 0;
	GB_Byte keys;
	struct SpriteInfo {
		GB_Byte y{ 0 };
		GB_Byte x{ 0 };
		GB_Byte tile{ 0 };
		GB_Byte flags{ 0 };

		SpriteInfo(int id);
	};
	

	void drawLine(int ly);
	std::vector<SpriteInfo> getSprites(int ly, bool big_sprites);
	void resetInterruptFlags();
	void SetMode(int mode);
	void updateLyc();
	
	
private:
	SDL_Window *MainWindow;
	SDL_Surface *MainSurface;
	SDL_Renderer *MainRenderer;
	SDL_Texture *MainTexture;
	SDL_Event e;
	
	SDL_Rect position_;
	uint32_t timer_{ 0 };

	int width;
	int height;
	
	Uint32* pixel;
	Uint32 color;
	int done{ 0 };
	int InnerClock=0;


};

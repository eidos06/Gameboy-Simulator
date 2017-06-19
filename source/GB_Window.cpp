#include "GB_Window.h"
#include "GB_Type.h"
#include <iostream>
#include<fstream>
#include "GB_Bits.h"
#include "GB_Const.h"
#include <vector>
#include <algorithm>
#include <Windows.h>
//init colormap
int ColorMap[] = { 255,200,100,0 };
using namespace std;

void GB_Window::create(int WINDOW_WIDTH , int WINDOW_HEIGHT,int x,int y, string WINDOW_TITLE)
{
	//create window
	SDL_Init(SDL_INIT_VIDEO);
	int cx = GetSystemMetrics(SM_CXFULLSCREEN)/2-80; int cy = GetSystemMetrics(SM_CYFULLSCREEN)/2-72;
	MainWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), cx, cy, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	width = WINDOW_WIDTH;
	height = WINDOW_HEIGHT;
	MainSurface = SDL_GetWindowSurface(MainWindow);
	Uint32* pixel = (Uint32*)MainSurface->pixels;
	//fill with some colors
	SDL_FillRect(MainSurface, NULL,SDL_MapRGB(MainSurface->format, 231, 0, 123));
	

}

int GB_Window::fresh()
{	
	//check the keyboard input
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			done = 1;
		//press key
		if ((e.type == SDL_KEYDOWN)) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE://quit the game
				done = 1;
				break;
			case SDLK_RIGHT://right
				memory_.KeysC1 &= 0xE;
				break;
			case SDLK_LEFT://left
				memory_.KeysC1 &= 0xD;
				break;
			case SDLK_UP://up
				memory_.KeysC1 &= 0xB;
				break;
			case SDLK_DOWN://down
				memory_.KeysC1 &= 0x7;
				break;
			case SDLK_z://A
				memory_.KeysC0 &= 0xE;
				break;
			case SDLK_x://B
				memory_.KeysC0 &= 0xD;
				break;
			case SDLK_SPACE://SELECT
				memory_.KeysC0 &= 0xB;
				break;
			case SDLK_RETURN://ENTER
				memory_.KeysC0 &= 0x7;
				break;
			}
		}
		//Key up
		if ((e.type == SDL_KEYUP)) {
			switch (e.key.keysym.sym) {
			case SDLK_RIGHT://right
				memory_.KeysC1 |= 0x1;
				break;
			case SDLK_LEFT://left
				memory_.KeysC1 |= 0x2;
				break;
			case SDLK_UP://up
				memory_.KeysC1 |= 0x4;
				break;
			case SDLK_DOWN://down
				memory_.KeysC1 |= 0x8;
				break;
			case SDLK_z://A
				memory_.KeysC0 |= 0x1;
				break;
			case SDLK_x://B
				memory_.KeysC0 |= 0x2;
				break;
			case SDLK_SPACE://SELECT
				memory_.KeysC0 |= 0x4;
				break;
			case SDLK_RETURN://ENTER
				memory_.KeysC0 |= 0x8;
				break;
			}
		}

	
	}
	if (done == 1)
	{
		return 0;//Close the window
	}
	return 1;
}

void GB_Window::end()
{
	SDL_FreeSurface(MainSurface);
	SDL_Quit();
}


//GPU Timer
void GB_Window::AddTime(int clocks) {
	resetInterruptFlags();
	//load LCDC data
	GB_Byte lcdc = memory_.ReadByte(LCDC_ADDRESS);
	//increase GPU Timer
	InnerClock += clocks;
	
	/*
	
	Modes Follow the order:
	OAM -> VRAM -> HBlank -> VBlank

	in OAM and VRAM, in fact program has nothing to do
	in Hblank, program read each line's data and put them into screen buffer
	in Vblank, program fresh the screen and update screen buffer
	
	*/

	if (NowMode == MODE_OAM) {
		if (InnerClock >= 79) {
			InnerClock -= 79;
			SetMode(MODE_VRAM);
		}
	}
	else if (NowMode == MODE_VRAM) {
		if (InnerClock >= 172) {
			InnerClock -= 172;
			SetMode(MODE_HBLANK);

			
		}
	}
	else if (NowMode == MODE_HBLANK) {

		if (InnerClock >= 205) {
			InnerClock -= 205;
			//read current line
			GB_Byte ly = memory_.ReadByte(LY_ADDRESS);
			//put line data into buffer
			drawLine(ly);
			//increase the ly
			ly++;
			//write ly into register
			memory_.WriteByte(LY_ADDRESS, ly);
			// if ly>=144, move to VBlank mode and update buffer and update screen. 
			if (ly >= 144) {
				SetMode(MODE_VBLANK);
				draw();
			}
			else {
				SetMode(MODE_OAM);
			}
		}
	}
	else {
		//10 cycles VBlank
		GB_Byte ly = (InnerClock / 456) + 144;
		if (InnerClock>= 4560) {
			//if 10 cycles done, ly=0 and move to H blank
			SetMode(MODE_OAM);
			InnerClock -= 4560;
			memory_.WriteByte(LY_ADDRESS, 0);
		}
		else {
			memory_.WriteByte(LY_ADDRESS, ly);
		}
	}
	//update LYC data
	updateLyc();
}



void GB_Window::setPixel(int x, int y, int color) {
	//this function set (x,y) pixel's color
	SDL_UnlockSurface(MainSurface);
	auto format = MainSurface->format;
	Uint32* pixels = (Uint32*)MainSurface->pixels;
	pixels[y * 160 + x] = SDL_MapRGB(format, color, color, color);
}

void GB_Window::draw() {
	SDL_UpdateWindowSurface(MainWindow);
}

GB_Window::SpriteInfo::SpriteInfo(int id)
	: y(memory_.ReadByte(0xFE00 + id * 4 + 0)),
	x(memory_.ReadByte(0xFE00 + id * 4 + 1)),
	tile(memory_.ReadByte(0xFE00 + id * 4 + 2)),
	flags(memory_.ReadByte(0xFE00 + id * 4 + 3)) {}

void GB_Window::drawLine(int ly) {
	if (ly >= 144) {
		return;
	}
	/*
	this lamda function read colors by bit
	example:
	AData:      00011010
	BData:      10100111
	Final Color:10122131

	*/
	auto color_number = [](int bit, GB_Byte AData, GB_Byte BData) -> int {
		return (((AData >> bit) & 1) << 1) | ((BData >> bit) & 1);
	};
	//read LCDC data
	GB_Byte lcdc = memory_.ReadByte(LCDC_ADDRESS);
	//Read Background X and Y position
	GB_Byte scx = memory_.ReadByte(SCX_ADDRESS);
	GB_Byte scy = memory_.ReadByte(SCY_ADDRESS);
	//determine which tile map to use
	bool signed_tile = GetBit(lcdc, 4);
	int BGTileData = !signed_tile ? 0x9000 : 0x8000;
	int BGTileMap = !GetBit(lcdc, 3) ? 0x9800 : 0x9C00;

	// Put BackGround into buffer
	if (GetBit(lcdc,0)) {
		int y = (ly + scy) % 256;//locate window in 256*256 map
		GB_Byte AData = 0x00;
		GB_Byte BData = 0x00;
		int LastTileX = -1;

		for (int i = 0; i < 160; i++) 
		{
			int x = (i + scx) % 256;
			int tileX = x / 8;//8 pixels / tile
			int tileY = y / 8;
			int pixelX = 8 - x % 8 - 1;
			int pixelY = y % 8;

			if (tileX != LastTileX) //a new tile to render
			{
				int tmp = memory_.ReadByte(BGTileMap + (tileY * 32) + tileX);//get tile No.
				int TileNo = tmp;
				if (!signed_tile) 
				{
					TileNo = (GB_SByte)tmp;//judge whether is signed or unsigned
				}
				AData = memory_.ReadByte(BGTileData + (TileNo * 16) + (pixelY * 2));
				BData = memory_.ReadByte(BGTileData + (TileNo * 16) + (pixelY * 2) + 1);
				LastTileX = tileX;
			}
			int color = color_number(pixelX, AData,BData);//get pixel color
			setPixel(i, ly, ColorMap[color]);//set color
		}
	}
	
	//Put Sprites into buffer
	vector<SpriteInfo> SpritesDatas = getSprites(ly);
	int size = SpritesDatas.size();
	if (size >= 10)
		size = 10;
	for (int i = size - 1; i >= 0; i--) 
	{
		SpriteInfo Spriteinfo = SpritesDatas[i];
		int PixelY = ly - Spriteinfo.y + 16;//fit actually Y
		bool ReverseX = GetBit(Spriteinfo.flags, 5);
		bool ReverseY = GetBit(Spriteinfo.flags, 6);
		bool Priority = GetBit(Spriteinfo.flags, 7);

		GB_Byte SpriteTile = Spriteinfo.tile;//get tile No.
		SpriteTile = Spriteinfo.tile | 0x01;
		PixelY -= 8;//originally -8
		
		if (ReverseY) {
				PixelY = 8 - PixelY - 1;	//up set down
			}

		GB_Byte AData = memory_.ReadByte(0x8000 + SpriteTile * 16 +
				PixelY * 2);
		GB_Byte BData = memory_.ReadByte(0x8000 + SpriteTile *16 +
				PixelY * 2 + 1);

		for (int x = 0; x < 8; x++)
		{
			if (Spriteinfo.x + x - 8 < 0)//out of range
			{
				continue;
			}
			int PixelX = 8 - x % 8 - 1;
			if (ReverseX)
			{
				PixelX = 8 - PixelX - 1;//up set down
			}

			int color = color_number(PixelX, AData, BData);
			setPixel(Spriteinfo.x + x - 8, ly, ColorMap[color]);
				
			}
		}
	}

void GB_Window::resetInterruptFlags() {
	GB_Byte interrupts = memory_.ReadByte(IF_ADDRESS);
	SetBit(interrupts, 0, false);
	SetBit(interrupts, 1, false);
	memory_.WriteByte(IF_ADDRESS, interrupts);
}

void GB_Window::SetMode(int mode) {
	if (mode == NowMode) {
		return;
	}
	NowMode = mode;

	GB_Byte stat = memory_.ReadByte(STAT_ADDRESS);
	// Write current mode
	stat &= 0xFC;
	stat |= NowMode & 0x03;

	// Request interrupt for mode if available
	GB_Byte interrupts = memory_.ReadByte(IF_ADDRESS);
	if ((mode == MODE_HBLANK && GetBit(stat, 3)) ||
		(mode == MODE_VBLANK && GetBit(stat, 4)) ||
		(mode ==MODE_OAM && GetBit(stat, 5))) {
		SetBit(interrupts, 1, true);
	}
	if (mode == MODE_VBLANK) {
		SetBit(interrupts, 0, true);
	}

	memory_.WriteByte(IF_ADDRESS, interrupts);
	memory_.WriteByte(STAT_ADDRESS, stat);
}

void GB_Window::updateLyc() {
	//read LY and LYC information
	GB_Byte ly = memory_.ReadByte(LY_ADDRESS);
	GB_Byte lyc = memory_.ReadByte(LYC_ADDRESS);
	//read STAT
	GB_Byte stat = memory_.ReadByte(STAT_ADDRESS);
	//set STAT
	SetBit(stat, 2, ly == lyc);
	
}

vector<GB_Window::SpriteInfo> GB_Window::getSprites(int ly) {
	vector<SpriteInfo> sprites;
	for (int i = 0; i < 40; i++) {
		//get No.i sprite info
		SpriteInfo it = SpriteInfo{ i };
		//if in this line, render it
		if(!(it.y == 0 || it.y >= 160 || ly <= it.y - 16 || ly >= it.y))
		{
			sprites.push_back(it);
		}
		
	}
	//sort sprites by X position
	sort(sprites.begin(), sprites.end(),
		[](const SpriteInfo& left, const SpriteInfo& right) {
		return left.x < right.x;
	});
	return sprites;
}
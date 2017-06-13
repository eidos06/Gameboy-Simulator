#include "GB_Window.h"
#include "GB_Type.h"
#include <iostream>
#include<fstream>
#include "GB_Bits.h"
#include "GB_Const.h"
#include <vector>
#include <algorithm>
int ColorMap[] = { 255,200,100,0 };
using namespace std;
void GB_Window::create(int WINDOW_WIDTH , int WINDOW_HEIGHT,int x,int y, string WINDOW_TITLE)
{
	SDL_Init(SDL_INIT_VIDEO);
	MainWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), x, y, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	width = WINDOW_WIDTH;
	height = WINDOW_HEIGHT;
	//MainRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_SOFTWARE);
	MainSurface = SDL_GetWindowSurface(MainWindow);
	//MainTexture = SDL_CreateTexture(MainRenderer, SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STREAMING,WINDOW_WIDTH,WINDOW_HEIGHT);
	Uint32* pixel = (Uint32*)MainSurface->pixels;
	SDL_FillRect(MainSurface, NULL,SDL_MapRGB(MainSurface->format, 231, 0, 123));
	SDL_Log("Window Created!\n");

}
int GB_Window::fresh()
{
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			done = 1;
		if ((e.type == SDL_KEYDOWN)) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				done = 1;
				break;
			case SDLK_RIGHT:
				memory_.KeysC1 &= 0xE;
				cout << "right" <<(int)keys<< endl;
				break;
			case SDLK_LEFT:
				memory_.KeysC1 &= 0xD;
				cout << "left" << endl;
				break;
			case SDLK_UP:
				memory_.KeysC1 &= 0xB;
				cout << "up" << endl;
				break;
			case SDLK_DOWN:
				memory_.KeysC1 &= 0x7;
				cout << "down" << endl;
				break;
			case SDLK_a:
				memory_.KeysC0 &= 0xE;
				cout << "a" << endl;
				break;
			case SDLK_b:
				memory_.KeysC0 &= 0xD;
				cout << "b" << endl;
				break;
			case SDLK_SPACE:
				memory_.KeysC0 &= 0xB;
				cout << "space" << endl;
				break;
			case SDLK_RETURN:
				memory_.KeysC0 &= 0x7;
				cout << "enter" << endl;
				break;
			}
		}
		if ((e.type == SDL_KEYUP)) {
			switch (e.key.keysym.sym) {
			case SDLK_RIGHT:
				memory_.KeysC1 |= 0x1;
				break;
			case SDLK_LEFT:
				memory_.KeysC1 |= 0x2;
				break;
			case SDLK_UP:
				memory_.KeysC1 |= 0x4;
				break;
			case SDLK_DOWN:
				memory_.KeysC1 |= 0x8;
				break;
			case SDLK_a:
				
				memory_.KeysC0 |= 0x1;
				break;
			case SDLK_b:
				
				memory_.KeysC0 |= 0x2;
				break;
			case SDLK_SPACE:
				
				memory_.KeysC0 |= 0x4;
				break;
			case SDLK_RETURN:
				
				memory_.KeysC0 |= 0x8;
				break;
			}
		}

	
	}
	if (done == 1)
	{
		return 0;
	}
	return 1;
}

void GB_Window::end()
{
	SDL_FreeSurface(MainSurface);
	SDL_Quit();
}



void GB_Window::AddTime(int clocks) {
	resetInterruptFlags();
	
	GB_Byte lcdc = memory_.ReadByte(LCDC_ADDRESS);
	InnerClock += clocks;

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
			GB_Byte ly = memory_.ReadByte(LY_ADDRESS);
			drawLine(ly);
			ly++;
			//cout << "HBLANK:" << (int)ly << endl;
			memory_.WriteByte(LY_ADDRESS, ly);
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
		GB_Byte ly = (InnerClock / 456) + 144;
		if (InnerClock>= 4560) {
			SetMode(MODE_OAM);
			InnerClock -= 4560;
			memory_.WriteByte(LY_ADDRESS, 0);
		}
		else {
			memory_.WriteByte(LY_ADDRESS, ly);
		}
	}

	updateLyc();
}



void GB_Window::setPixel(int x, int y, int color) {
	SDL_UnlockSurface(MainSurface);
	auto format = MainSurface->format;
	Uint32* pixels = (Uint32*)MainSurface->pixels;
	pixels[y * 160 + x] = SDL_MapRGB(format, color, color, color);
}

void GB_Window::draw() {
	/*
	SDL_UpdateTexture(MainTexture, NULL, MainSurface->pixels, MainSurface->pitch);
	
	if (SDL_RenderClear(MainRenderer)) {
		return;
	}
	if (SDL_RenderCopy(MainRenderer, MainTexture, &position_, NULL)) {
		return;
	}

	SDL_RenderPresent(MainRenderer);
	*/
	
	SDL_UpdateWindowSurface(MainWindow);
	counts++;
	//cout << counts << endl;
	
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
	//GB_Byte bgp_data = memory_.ReadByte(BGP_ADDRESS);
	//GB_Byte obp0_data = memory_.ReadByte(OBP0_ADDRESS);
	//GB_Byte obp1_data = memory_.ReadByte(OBP1_ADDRESS);

	
	auto color_number = [](int bit, GB_Byte AData, GB_Byte BData) -> int {
		return (((AData >> bit) & 1) << 1) | ((BData >> bit) & 1);
	};

	GB_Byte lcdc = memory_.ReadByte(LCDC_ADDRESS);
	GB_Byte scx = memory_.ReadByte(SCX_ADDRESS);
	//cout << "scx"<<(int)scx << endl;
	GB_Byte scy = memory_.ReadByte(SCY_ADDRESS);
	//cout << "scy"<<(int)scy << endl;
	
	bool signed_tile = GetBit(lcdc, 4);
	int BGTileData = !signed_tile ? 0x9000 : 0x8000;
	int BGTileMap = !GetBit(lcdc, 3) ? 0x9800 : 0x9C00;

	// BackGround
	
	if (GetBit(lcdc,0)) {
		int y = (ly + scy) % 256;
		GB_Byte AData = 0x00;
		GB_Byte BData = 0x00;
		int LastTileX = -1;

		for (int i = 0; i < 160; i++) {
			int x = (i + scx) % 256;
			int tileX = x / 8;
			int tileY = y / 8;
			int pixelX = 8 - x % 8 - 1;
			int pixelY = y % 8;

			if (tileX != LastTileX) {
				
				int tmp = memory_.ReadByte(BGTileMap + (tileY * 32) + tileX);
				int TileNo = tmp;
				if (!signed_tile) {
					TileNo = (GB_SByte)tmp;
				}
				AData = memory_.ReadByte(BGTileData + (TileNo * 16) + (pixelY * 2));
				BData = memory_.ReadByte(BGTileData + (TileNo * 16) + (pixelY * 2) + 1);
				LastTileX = tileX;
			}

			int color = color_number(pixelX, AData,BData);
			setPixel(i, ly, ColorMap[color]);
		}
	}
	
	//Sprites

	
	vector<SpriteInfo> SpritesDatas = getSprites(ly);
	int size = SpritesDatas.size();
	if (size >= 10)
		size = 10;
	for (int i = size - 1; i >= 0; i--) {
		SpriteInfo Spriteinfo = SpritesDatas[i];
		int PixelY = ly - Spriteinfo.y + 16;
		bool ReverseX = GetBit(Spriteinfo.flags, 5);
		bool ReverseY = GetBit(Spriteinfo.flags, 6);
		bool Priority = GetBit(Spriteinfo.flags, 7);

		GB_Byte SpriteTile = Spriteinfo.tile;
		SpriteTile =Spriteinfo.tile | 0x01;
		PixelY -= 8;
		
		if (ReverseY) {
				PixelY = 8 - PixelY - 1;
				
			}

			GB_Byte AData = memory_.ReadByte(0x8000 + SpriteTile * 16 +
				PixelY * 2);
			GB_Byte BData = memory_.ReadByte(0x8000 + SpriteTile *16 +
				PixelY * 2 + 1);

			for (int x = 0; x < 8; x++) {
				if (Spriteinfo.x + x - 8 < 0) {
					continue;
				}
				int PixelX = 8 - x % 8 - 1;
				if (ReverseX) {
					PixelX = 8 - PixelX - 1;
				}

				int color = color_number(PixelX, AData, BData);
				setPixel(i, ly, ColorMap[color]);
				
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
	GB_Byte ly = memory_.ReadByte(LY_ADDRESS);
	GB_Byte lyc = memory_.ReadByte(LYC_ADDRESS);

	GB_Byte stat = memory_.ReadByte(STAT_ADDRESS);
	SetBit(stat, 2, ly == lyc);

	if (ly == lyc) {
		GB_Byte interrupts = memory_.ReadByte(IF_ADDRESS);
		SetBit(interrupts, 1, true);
		memory_.WriteByte(IF_ADDRESS, interrupts);
	}
}

vector<GB_Window::SpriteInfo> GB_Window::getSprites(int ly) {
	vector<SpriteInfo> sprites;
	for (int i = 0; i < 40; i++) {
		SpriteInfo it = SpriteInfo{ i };
		if(!(it.y == 0 || it.y >= 160 || ly <= it.y - 16 || ly >= it.y))
		//{
			sprites.push_back(it);
		//}
		
	}

	
	std::sort(sprites.begin(), sprites.end(),
		[](const SpriteInfo& left, const SpriteInfo& right) {
		return left.x < right.x;
	});
	cout << sprites.size() << endl;
	return sprites;
}
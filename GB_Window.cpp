#include "GB_Window.h"
#include "GB_Type.h"
#include <iostream>
#include<fstream>
#include "GB_Bits.h"
#include "GB_Const.h"
int ColorMap[] = { 255,200,100,0 };

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
				SetBit(keys, 0, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "right" <<(int)keys<< endl;
				break;
			case SDLK_LEFT:
				SetBit(keys, 1, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "left" << endl;
				break;
			case SDLK_UP:
				SetBit(keys, 2, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "up" << endl;
				break;
			case SDLK_DOWN:
				SetBit(keys, 3, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "down" << endl;
				break;
			case SDLK_a:
				SetBit(keys, 4, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "a" << endl;
				break;
			case SDLK_b:
				SetBit(keys, 5, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "b" << endl;
				break;
			case SDLK_SPACE:
				SetBit(keys, 6, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "space" << endl;
				break;
			case SDLK_RETURN:
				SetBit(keys, 7, true);
				memory_.WriteByte(0xFF00, keys);
				cout << "enter" << endl;
				break;
			}
		}
		if ((e.type == SDL_KEYUP)) {
			switch (e.key.keysym.sym) {
			case SDLK_RIGHT:
				SetBit(keys, 0, false);
				memory_.WriteByte(0xFF00, keys);
				break;
			case SDLK_LEFT:
				SetBit(keys, 1, false);
				memory_.WriteByte(0xFF00, keys);
				break;
			case SDLK_UP:
				SetBit(keys, 2, false);
				memory_.WriteByte(0xFF00, keys);
				break;
			case SDLK_DOWN:
				SetBit(keys, 3, false);
				memory_.WriteByte(0xFF00, keys);
				break;
			case SDLK_a:
				SetBit(keys, 4, false);
				memory_.WriteByte(0xFF00, keys);
				break;
			case SDLK_b:
				SetBit(keys, 5, false);
				memory_.WriteByte(0xFF00, keys);
				break;
			case SDLK_SPACE:
				SetBit(keys, 6, false);
				memory_.WriteByte(0xFF00, keys);
				break;
			case SDLK_RETURN:
				SetBit(keys, 7, false);
				memory_.WriteByte(0xFF00, keys);
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

	auto palette = [](GB_Byte palette, int color) -> int {
		int value = palette >> (color * 2);
		value &= 0x3;
		return ColorMap[value];
	};
	auto color_number = [](int bit, GB_Byte AData, GB_Byte BData) -> int {
		return (((AData >> bit) & 1) << 1) | ((BData >> bit) & 1);
	};

	GB_Byte lcdc = memory_.ReadByte(LCDC_ADDRESS);
	GB_Byte scx = memory_.ReadByte(SCX_ADDRESS);
	GB_Byte scy = memory_.ReadByte(SCY_ADDRESS);

	bool signed_tile = GetBit(lcdc, 4);
	int BGTileData = !signed_tile ? 0x9000 : 0x8000;
	int BGTileMap = !GetBit(lcdc, 3) ? 0x9800 : 0x9C00;

	// BG
	vector<int> bgcolors(160, 0);
	if (1==1) {
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
			//bgcolors[i] = color;
			//GB_Byte pixel = palette(bgp_data, color);
			setPixel(i, ly, ColorMap[color]);
		}
	}
	else {
		for (int i = 0; i < 160; i++) {
			setPixel(i, ly, 0);
		}
	}
	/*
	Byte wx = memory_.read(Register::Wx);
	Byte wy = memory_.read(Register::Wy);
	Word win_tile_map = !bits::bit(lcdc, 6) ? 0x9800 : 0x9C00;
	if (bits::bit(lcdc, 5) && wx <= 166 && wy <= ly) {
		int y = ly - wy;
		Byte bottom = 0x00;
		Byte top = 0x00;
		int last_tile_x = -1;

		for (int i = wx - 7; i < 160; i++) {
			int x = i - wx + 7;
			int tile_x = x / 8;
			int tile_y = y / 8;
			int pixel_x = 8 - x % 8 - 1;
			int pixel_y = y % 8;

			if (tile_x != last_tile_x) {
				int tile = memory_.read(win_tile_map + (tile_y * 32) + tile_x);
				int offset = tile;
				if (!signed_tile) {
					offset = static_cast<SByte>(tile);
				}
				bottom = memory_.read(bg_tile_data + (offset * 16) + (pixel_y * 2));
				top = memory_.read(bg_tile_data + (offset * 16) + (pixel_y * 2) + 1);
				last_tile_x = tile_x;
			}

			int color = color_number(pixel_x, top, bottom);
			bgcolors[i] = color;
			Byte pixel = palette(bgp_data, color);
			window_.setPixel(i, ly, pixel);
		}
	}

	// OBJ
	bool big_sprites = bits::bit(lcdc, 2);
	std::vector<SpriteInfo> sprites = getSprites(ly, big_sprites);
	// Keep at most 10 sprites
	int size = std::min(10, static_cast<int>(sprites.size()));
	// Draw them! Back to front, so front renders on top.
	for (int i = size - 1; i >= 0; i--) {
		SpriteInfo& info = sprites[i];
		int pixel_y = ly - info.y + 16;
		int sprite_count = big_sprites ? 2 : 1;

		Byte obp = bits::bit(info.flags, 4) ? obp1_data : obp0_data;
		bool reverse_x = bits::bit(info.flags, 5);
		bool reverse_y = bits::bit(info.flags, 6);
		bool behind = bits::bit(info.flags, 7);

		for (int sprites = 0; sprites < sprite_count; sprites++) {
			Byte sprite_tile = info.tile;

			// If we're in big sprite mode, we select our tiles a bit differently.
			if (big_sprites) {
				if (sprites == 0) {
					sprite_tile = info.tile & 0xFE;
				}
				else {
					sprite_tile = info.tile | 0x01;
					pixel_y -= 8;
				}
			}

			if (reverse_y) {
				pixel_y = 8 - pixel_y - 1;
				// In big sprite mode we have to flip the two tiles
				if (big_sprites) {
					pixel_y += sprites == 1 ? -8 : 8;
				}
			}

			Byte bottom = memory_.read(0x8000 + (static_cast<int>(sprite_tile) * 16) +
				(pixel_y * 2));
			Byte top = memory_.read(0x8000 + (static_cast<int>(sprite_tile) * 16) +
				(pixel_y * 2) + 1);
			for (int x = 0; x < 8; x++) {
				if (info.x + x - 8 < 0) {
					continue;
				}
				int pixel_x = 8 - x % 8 - 1;
				if (reverse_x) {
					pixel_x = 8 - pixel_x - 1;
				}

				int color = color_number(pixel_x, top, bottom);
				if (color != 0 && !(behind && bgcolors[info.x + x - 8] > 0)) {
					Byte pixel = palette(obp, color);
					window_.setPixel(info.x + x - 8, ly, pixel);
				}
			}
		}
	}
	*/
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
#pragma once
#include <SDL.h>
#include <string>


using namespace std;
class GB_Window {
public:
	void create (int WINDOW_WIDTH, int WINDOW_HEIGHT, int x, int y, string WINDOW_TITLE);
	int fresh();
	void end();

private:
	SDL_Window *MainWindow;
	SDL_Surface *MainSurface;
	SDL_Renderer *MainRenderer;
	SDL_Texture *MainTexture;
	SDL_Event e;
	int done{ 0 };
};

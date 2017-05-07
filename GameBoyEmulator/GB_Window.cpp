#include "GB_Window.h"


void GB_Window::create(int WINDOW_WIDTH , int WINDOW_HEIGHT,int x,int y, string WINDOW_TITLE)
{
	SDL_Init(SDL_INIT_VIDEO);
	MainWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), x, y, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	MainSurface = SDL_GetWindowSurface(MainWindow);
	MainRenderer = SDL_CreateRenderer(MainWindow, -1, SDL_RENDERER_SOFTWARE);
	MainTexture = SDL_CreateTexture(MainRenderer, SDL_PIXELFORMAT_ABGR8888,SDL_TEXTUREACCESS_STREAMING,WINDOW_WIDTH,WINDOW_HEIGHT);
	SDL_Log("Window Created!\n");

}
int GB_Window::fresh()
{
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
			done = 1;
		if ((e.type == SDL_KEYDOWN) && (e.key.keysym.sym == SDLK_ESCAPE)) {
			done = 1;
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
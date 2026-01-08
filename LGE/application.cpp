#include "application.h"


lge::Application::Application()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::println("Couldn't initialize SDL: {}", SDL_GetError());
		//return 1;
	}
}

lge::Application::~Application()
{
	MainWindow.~LgeWindow();
}

void lge::Application::PollEvents()
{
	while (SDL_PollEvent(&Event))
	{
		switch (Event.type)
		{
		case SDL_EVENT_QUIT:
			bShouldClose = true;
			break;
		}
	}
}

SDL_Window* lge::Application::GetMainWindow()
{
	return MainWindow.GetWindow();
}

////////////////
// LGE WINDOW //
////////////////

lge::LgeWindow::LgeWindow(int Width, int Height, std::string WindowName) : WindowWidth{ Width }, WindowHeight{ Height }
{
	Window = SDL_CreateWindow(WindowName.c_str(), WindowWidth, WindowHeight, 0);
}

SDL_Window* lge::LgeWindow::GetWindow()
{
	return Window;
}


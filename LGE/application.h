#pragma once

#include "imgui_impl_bgfx.h"
#include "imgui_impl_sdl3.h"
#include <SDL3/SDL.h>
#include <bgfx/bgfx.h> // For bgfx::RendererType::Enum
#include <string>
#include <print>

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

namespace lge 
{
	
	class LgeWindow
	{
	public:
		LgeWindow(int Width, int Height, std::string WindowName);
		~LgeWindow() { SDL_DestroyWindow(Window); };

		SDL_Window* GetWindow();

	private:
		int WindowWidth;
		int WindowHeight;

		SDL_Window* Window;
	};

	class ImGuiLayer
	{
		public:
			ImGuiLayer() = default;
			~ImGuiLayer() = default;
			//void Initialize(SDL_Window* window);
			//void BeginFrame();
			//void EndFrame();
			//void Shutdown();
		private:
			//SDL_Window* AttachedWindow;

	};

	// APPLICATION CLASS //

	class Application
	{
	public:
		Application();
		~Application();

		void PollEvents();

		// Getters
		SDL_Window* GetMainWindow();
		bool ShouldClose() { return bShouldClose; };
	private:
		LgeWindow MainWindow{ SCREEN_WIDTH, SCREEN_HEIGHT, "LGE" };
		bool bShouldClose = false;
		SDL_Event Event;
	};
}


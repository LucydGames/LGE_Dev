#pragma once


#include "imgui_impl_sdl3.h"
#include "imgui.h"
#include "log.h"
#include "meta.h"
#include <SDL3/SDL.h>
//#include <bgfx/bgfx.h> // For bgfx::RendererType::Enum
#include <string>
#include <print>

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

	class ImGuiLayer // Make seperate file. Initialization of imgui handled sperately in application. Layer class will be for rendering UI groups or layers in virtual function
	{
		public:
			ImGuiLayer();
			~ImGuiLayer() {};
			bool Initialize(SDL_Window* window);
			void ProcessEvent(SDL_Event& event);
			void BeginFrameSDL();
			void EndFrame();
			void Shutdown();
			void ShowDemoWindow();
			bool IsInitialized() { return bIsInitialized; };
		private:
			//SDL_Window* AttachedWindow;
			bool bIsInitialized = false;

	};

	// APPLICATION CLASS //

	class Application
	{
	public:
		Application();
		~Application();

		bool InitializeLayers();

		void RenderLayers();

		void BeginFrame();
		void EndFrame();

		void PollEvents();

		// Getters
		SDL_Window* GetMainWindow();
		ImGuiLayer ImGuiLayer;
		SDL_Event& GetEvent() { return Event; };
		bool ShouldClose() { return bShouldClose; };
	private:
		LgeWindow MainWindow{ SCREEN_WIDTH, SCREEN_HEIGHT, "LGE" };
		bool bShouldClose = false;
		bool bImGuiInitialized = false;
		SDL_Event Event;
	};
}


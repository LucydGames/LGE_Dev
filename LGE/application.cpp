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

lge::ImGuiLayer::ImGuiLayer()
{
	
}

bool lge::ImGuiLayer::Initialize(SDL_Window* window, uint16_t ViewId, bgfx::RendererType::Enum RenderAPI)
{
	//ImGui_ImplSDL3_InitForBGFX(window);
	bool result = false;
	
	switch (RenderAPI)
	{
	case bgfx::RendererType::OpenGL:
		result = ImGui_ImplSDL3_InitForOpenGL(window, nullptr);
		break;
	case bgfx::RendererType::Vulkan:
		result = ImGui_ImplSDL3_InitForVulkan(window);
		break;
	case bgfx::RendererType::Direct3D11:
		result = ImGui_ImplSDL3_InitForD3D(window);
		break;
	default:
		break;
	}
	ImGui_ImplBgfx_Init(ViewId);
	return result;
}

void lge::ImGuiLayer::BeginFrame()
{
	ImGui_ImplSDL3_NewFrame();
	ImGui_ImplBgfx_NewFrame();
}

void lge::ImGuiLayer::EndFrame()
{
	ImGui::Render();
}

void lge::ImGuiLayer::Shutdown()
{
	ImGui_ImplBgfx_Shutdown();
	ImGui_ImplSDL3_Shutdown();
}
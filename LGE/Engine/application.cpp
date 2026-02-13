#include "application.h"
#include "renderer.h"


lge::Application::Application()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		Log::GetInstance().PrintError("Couldn't initialize SDL:");
		Log::GetInstance().PrintError(SDL_GetError());
		//std::println("Couldn't initialize SDL: {}", SDL_GetError());
	}
	Renderer::Initialize(MainWindow.GetWindow(), bgfx::RendererType::Count);
}

lge::Application::~Application()
{
	ImGuiLayer.Shutdown();
	MainWindow.~LgeWindow();
}


bool lge::Application::InitializeLayers()
{
	bool ResultSDL = false;

	ImGui::CreateContext();

	ResultSDL = ImGui_ImplSDL3_InitForOther(MainWindow.GetWindow());

	bImGuiInitialized = ResultSDL;

	return bImGuiInitialized;
}

void lge::Application::RenderLayers()
{
	ImGui::Render(); // DEPRECATE
}

void lge::Application::PollEvents()
{
	if (SDL_PollEvent(&Event))
	{
		if (bImGuiInitialized)
			ImGuiLayer.ProcessEvent(Event); // Switch to not use ImGuiLayer, deprecated
		switch (Event.type)
		{
		case SDL_EVENT_QUIT:
			bShouldClose = true;
			break;
		}
	}
}

void lge::Application::BeginFrame()
{
	ImGuiLayer.BeginFrameSDL();
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

bool lge::ImGuiLayer::Initialize(SDL_Window* window)
{
	
}

void lge::ImGuiLayer::ProcessEvent(SDL_Event& event)
{
	ImGui_ImplSDL3_ProcessEvent(&event);
}

void lge::ImGuiLayer::BeginFrameSDL()
{
	ImGui_ImplSDL3_NewFrame();
	
}

void lge::ImGuiLayer::EndFrame()
{
	//ImGui::Render();
	ImGui::End();
}

void lge::ImGuiLayer::Shutdown()
{
	ImGui_ImplSDL3_Shutdown();
}

void lge::ImGuiLayer::ShowDemoWindow()
{
	ImGui::ShowDemoWindow();
}

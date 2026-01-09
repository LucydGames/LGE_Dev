#include "renderer.h"
#include "application.h"
#include "imgui.h"
#include "imgui_impl_bgfx.h"
#include "imgui_impl_sdl3.h"
#include "log.h"

static const char* RenderApi[] = { "None", "Agc", "Direct3D11", "Direct3D12", "Gnm", "Metal", "Nvn", "OpenGLES", "OpenGL", "Vulkan", "Count"};
const char* GetRenderApiName(int enumVal)
{
	return RenderApi[enumVal];
}

int main()
{
	lge::Application Application;
	Log::GetInstance().PrintInfo("Application Started.");

	Renderer renderer;
	renderer.Initialize(Application.GetMainWindow(), bgfx::RendererType::Vulkan);
	Log::GetInstance().PrintInfo("Using Renderer API:");
	Log::GetInstance().PrintInfo(GetRenderApiName(renderer.GetRendererType())); // Format string to reduce lines

	Image* image = new Image("texture.png");
	bool running = true;
	float rotation = 0.0f;

	//lge::ImGuiLayer ImGui;
	//bool ImGuiResult = ImGui.Initialize(Application.GetMainWindow(), renderer.GetMainViewId(), renderer.GetRendererType());
	//if (!ImGuiResult)
	//{
	//	Log::GetInstance().PrintError("Failed to initialize ImGui Layer!");
	//	return -1;
	//}

	while (!Application.ShouldClose())
	{
		Application.PollEvents();

		renderer.Begin();
		renderer.DrawQuad({ -550.0f, 0.0f, 0.1f }, 0.0f, 0x7700ffff, { 1.0f, 1.0f, 1.0f }, 0);
		renderer.DrawQuad({ -500.0f, 0.0f, 0.2f }, 0.0f, 0xffff00ff, { 1.0f, 1.0f, 1.0f }, 1);

		renderer.DrawImage(image, { 0.0f, 200.0f, 0.0f }, rotation, 0xffffffff, 0);

		renderer.WriteToStencil();
		renderer.DrawQuad({ 500.0f, 0.0f, 0.0f }, 0.0f, 0xffff00ff, { 1.0f, 1.0f, 1.0 }, 0);
		renderer.EnableStencilTest();
		renderer.DrawQuad({ 500.0f, 0.0f, 0.0f }, 0.0f, 0xff00ffff, { 1.3f, 1.3f, 1.0 }, 0);
		renderer.DisableStencil();
		renderer.Render();
		rotation += 1.0f;
		rotation = std::fmod(rotation, 360.0f);
	}

	renderer.Shutdown();
	Application.~Application();
	return 0;
}
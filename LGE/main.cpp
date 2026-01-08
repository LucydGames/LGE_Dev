#include "renderer.h"
#include "application.h"
#include "imgui.h"
#include "imgui_impl_bgfx.h"
#include "imgui_impl_sdl3.h"

int main()
{
	lge::Application Application;
	

	Renderer renderer;
	renderer.Initialize(Application.GetMainWindow()/*, bgfx::RendererType::Vulkan*/);

	Image* image = new Image("texture.jpg");
	bool running = true;
	//SDL_Event event;
	float rotation = 0.0f;

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
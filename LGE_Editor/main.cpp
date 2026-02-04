#include "renderer.h"
#include "application.h"
#include "imgui.h"
#include "imgui_impl_bgfx.h"
#include "imgui_impl_sdl3.h"
#include "glm/gtc/type_ptr.hpp"
#include "lge_model.h"
#include "log.h"

#define LGE_EDITOR_BUILD

static const char* RenderApi[] = { "None", "Agc", "Direct3D11", "Direct3D12", "Gnm", "Metal", "Nvn", "OpenGLES", "OpenGL", "Vulkan", "Count"};
const char* GetRenderApiName(int enumVal)
{
	return RenderApi[enumVal];
}

int main()
{
	
	lge::Application Application;
	Log::GetInstance().PrintInfo("Application Started.");

	if (Application.InitializeLayers())
	{
		Log::GetInstance().PrintInfo("ImGui Layer Initialized.");
	}
	else
	{
		Log::GetInstance().PrintError("ImGui Layer Failed to Initialize.");
	}

	Renderer renderer;
	renderer.Initialize(Application.GetMainWindow(), bgfx::RendererType::Vulkan);
	renderer.InitializeImGui(UI_VIEW); // UI View
	Log::GetInstance().PrintInfo("Using Renderer API:");
	Log::GetInstance().PrintInfo(GetRenderApiName(renderer.GetRendererType())); // Format string to reduce lines

	Image* image = new Image("texture.png");
	Image* TireTexture = new Image("../LGE/assets/tex/hknife_Base_Color.png");
	bool running = true;
	float rotation = 0.0f;
	glm::vec4 PostProcessColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	glm::vec4 ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	bool bRotate = false;

	glm::vec3 modelPosition = { 0.0f, 0.0f, 200.0f };
	glm::vec3 modelScale = { 200.0f, 200.0f, 200.0f };

	std::shared_ptr<lge::LgeModel> model = lge::LgeModel::LoadModelFromFile("../LGE/assets/models/hknife.obj");

	while (!Application.ShouldClose())
	{
		Application.BeginFrame();
		Application.PollEvents();

		renderer.Begin();

		model->Draw(modelPosition, rotation, modelScale, renderer.m_Stencil, renderer.m_Uniform, TireTexture->GetTextureHandle(), renderer.m_ShaderProgram->GetProgramHandle());
		
		renderer.DrawQuad({ -550.0f, 0.0f, 0.1f }, 0.0f, 0x7700ffff, { 1.0f, 1.0f, 1.0f }, 0);
		renderer.DrawQuad({ -500.0f, 0.0f, 0.2f }, 0.0f, 0xffff00ff, { 1.0f, 1.0f, 1.0f }, 1);

		renderer.DrawImage(image, { 0.0f, 200.0f, 0.0f }, rotation, 0xffffffff, 0);

		renderer.WriteToStencil();
		renderer.DrawQuad({ 500.0f, 0.0f, 0.0f }, 0.0f, 0xffff00ff, { 1.0f, 1.0f, 1.0 }, 0);
		renderer.EnableStencilTest();
		renderer.DrawQuad({ 500.0f, 0.0f, 0.0f }, 0.0f, 0xff00ffff, { 1.3f, 1.3f, 1.0 }, 0);
		renderer.DisableStencil();

		ImGui::NewFrame();
		
		//ImGui::ShowDemoWindow();
		ImGui::Begin("LGE Editor");
		ImGui::Text("Hello, LGE Editor!");
		ImGui::Checkbox("Rotate Image", &bRotate);
		ImGui::SliderFloat("Rotation", &rotation, 0.0f, 360.0f);
		if (ImGui::ColorEdit4("PostProcess Color", glm::value_ptr(PostProcessColor)) == true)
		{
			uint8_t r = static_cast<uint8_t>(PostProcessColor.r * 255.0f);
			uint8_t g = static_cast<uint8_t>(PostProcessColor.g * 255.0f);
			uint8_t b = static_cast<uint8_t>(PostProcessColor.b * 255.0f);
			uint8_t a = static_cast<uint8_t>(PostProcessColor.a * 255.0f);
			uint32_t color = (a << 24) | (b << 16) | (g << 8) | r;
			renderer.SetPostProcessColor(color);
		}
		if (ImGui::ColorEdit4("Clear Color", glm::value_ptr(ClearColor)) == true) // Clear color 2D is blending with ClearColor from render view
		{
			uint8_t r = static_cast<uint8_t>(ClearColor.r * 255.0f);
			uint8_t g = static_cast<uint8_t>(ClearColor.g * 255.0f);
			uint8_t b = static_cast<uint8_t>(ClearColor.b * 255.0f);
			uint8_t a = static_cast<uint8_t>(ClearColor.a * 255.0f);
			uint32_t color = (a << 24) | (b << 16) | (g << 8) | r;
			renderer.SetClearColor2D(color);
		}
		ImGui::InputFloat3("Model Position", glm::value_ptr(modelPosition));
		ImGui::InputFloat3("Model Scale", glm::value_ptr(modelScale));
		ImGui::End();
		renderer.Render();
		
		if (bRotate)
			rotation += 1.0f;
		rotation = std::fmod(rotation, 360.0f);
	}

	//Application.ImGuiLayer.Shutdown(); // create layer Shutdown function in Application
	renderer.Shutdown();
	Application.~Application();
	return 0;
}
//#include <bx/bx.h> //temp
#define BX_CONFIG_DEBUG 1
#include <bx/math.h> // temp

//#include <bx/pixelformat.h> // temp
 //#include <ctime>

#include "renderer.h"
#include "application.h"
#include "imgui.h"
#include "imgui_impl_bgfx.h"
#include "imgui_impl_sdl3.h"
#include "glm/gtc/type_ptr.hpp"
#include "lge_model.h"
#include "log.h"

#include "lge_utils.h"

#define LGE_PLATFORM_WINDOWS
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
	renderer.Initialize(Application.GetMainWindow(), bgfx::RendererType::Count);
	renderer.InitializeImGui(UI_VIEW); // UI View
	Log::GetInstance().PrintInfo("Using Renderer API:");
	Log::GetInstance().PrintInfo(GetRenderApiName(renderer.GetRendererType())); // Format string to reduce lines

	Image* image = new Image("texture.png");
	Image* TireTexture = new Image("../LGE/assets/tex/hknife_Base_Color.png");
	Image* TireNormal = new Image("../LGE/assets/tex/hknife_Normal.png");
	bool running = true;
	float rotation = 0.0f;
	glm::vec4 PostProcessColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	glm::vec4 ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	glm::vec4 ClearColor3D = { 0.2f, 0.2f, 0.2f, 1.0f };
	bool bRotate = false;

	glm::vec3 modelPosition = { -0.5f, -1.0f, 1.0f };
	glm::vec3 modelScale = { 0.8f, 0.8f, 0.8f };

	std::shared_ptr<lge::LgeModel> model = lge::LgeModel::LoadModelFromFile("../LGE/assets/models/hknife.obj");

	ShaderProgram* modelShaderProgram = new ShaderProgram("../LGE/vs_bumpd3d.bin", "../LGE/fs_bumpd3d.bin");

	constexpr uint16_t numLights = 3;
	bgfx::UniformHandle u_lightPosRadius = bgfx::createUniform("u_lightPosRadius", bgfx::UniformType::Vec4, numLights);
	bgfx::UniformHandle u_lightRgbInnerR = bgfx::createUniform("u_lightRgbInnerR", bgfx::UniformType::Vec4, numLights);
	float lightPosRadius[numLights][4];
	glm::vec4 lightRgbInnerR[numLights];
	float light1PosX = -1.0f;
	float light2PosX = 0.0f;
	float light3PosX = 0.0f;
	bool lightReverse1 = false;
	bool lightReverse2 = false;
	bool lightReverse3 = false;

	while (!Application.ShouldClose())
	{
		Application.BeginFrame();
		Application.PollEvents();

		renderer.Begin();

		lightPosRadius[0][1] = 1.0f;
		lightPosRadius[1][1] = 0.0f;
		lightPosRadius[2][1] = -1.0f;
		
		lightPosRadius[3][1] = 0.0f; // center white light

		for (uint32_t ii = 0; ii < numLights; ++ii)
		{
			if (ii == 0)
				lightPosRadius[ii][0] = light1PosX += lightReverse1 ? -0.01f : 0.01f;
			else if (ii == 1)
				lightPosRadius[ii][0] = light2PosX += lightReverse2 ? -0.01f : 0.01f;
			else if (ii == 2)
				lightPosRadius[ii][0] = light3PosX += lightReverse3 ? -0.01f : 0.01f;
			//lightPosRadius[ii][0] = 1.0f;
			
			lightPosRadius[ii][2] = 0.5f;
			lightPosRadius[ii][3] = 2.0f;
		}
		lightPosRadius[3][2] = 0.0f; // I KNOW I KNOW, REWRITE REDUNDANT
		lightPosRadius[3][3] = 3.0f; // I KNOW I KNOW, REWRITE REDUNDANT
		
		lightRgbInnerR[0] = glm::vec4(1.0f, 0.5f, 5.0f, 0.1f);
		lightRgbInnerR[1] = glm::vec4(0.5f, 1.0f, 0.5f, 0.1f);
		lightRgbInnerR[2] = glm::vec4(0.5f, 0.5f, 1.0f, 0.1f);
		lightRgbInnerR[3] = glm::vec4(1.0f, 1.0f, 1.0f, 0.01f);
		

		bgfx::setUniform(u_lightPosRadius, lightPosRadius, numLights); // move to main
		bgfx::setUniform(u_lightRgbInnerR, lightRgbInnerR, numLights); // move to main

		model->Draw(modelPosition, rotation, modelScale, TireTexture->GetTextureHandle(), TireNormal->GetTextureHandle(), modelShaderProgram->GetProgramHandle());
		
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
		if (ImGui::ColorEdit4("Clear Color3d", glm::value_ptr(ClearColor3D)) == true)
		{
			renderer.SetClearColor3D(lge::EncodeRgbaToUint32Color(ClearColor3D.r, ClearColor3D.g, ClearColor3D.b, ClearColor3D.a));
		}
		ImGui::InputFloat3("Model Position", glm::value_ptr(modelPosition));
		ImGui::InputFloat3("Model Scale", glm::value_ptr(modelScale));
		ImGui::End();
		renderer.Render();
		
		if (bRotate)
			rotation += 1.0f;
		rotation = std::fmod(rotation, 360.0f);
		if (light1PosX > 1.0f)
			lightReverse1 = true;
		if (light2PosX > 1.0f)
			lightReverse2 = true;
		if (light3PosX > 1.0f)
			lightReverse3 = true;

		if (light1PosX < -1.0f)
			lightReverse1 = false;
		if (light2PosX < -1.0f)
			lightReverse2 = false;
		if (light3PosX < -1.0f)
			lightReverse3 = false;
	}

	//Application.ImGuiLayer.Shutdown(); // create layer Shutdown function in Application
	renderer.Shutdown();
	Application.~Application();
	return 0;
}
#pragma once

#include <print>
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <vector>
#include "meta.h"
#include "shader.h"
#include "image.h"
#include "frame_buffer.h"
#include "imgui_impl_bgfx.h"
#include "renderer_common.h"
#include "log.h"


// TODO: Create Viewport class. Viewports will manage their own frame buffers, camera matrices, and rendering passes.
// Main viewport will be init when renderer inits, and will handle things like clearing screen and post processing effects.

// Renderer will be static class and will be called by various classes such as application, scene (update scene/ render actors), ui layer etc.

namespace lge
{
	struct RenderBatch
	{
		glm::mat4 Transform;
		bgfx::TextureHandle Texture;
		uint32_t StartIndex;
		uint32_t NumberOfIndices;
		uint32_t Depth;
		uint32_t Stencil;
	};

	class Renderer
	{
	public:
		Renderer();
		static void Initialize(SDL_Window* window, bgfx::RendererType::Enum RendererApiType = bgfx::RendererType::Count);
		void InitializeImGui(int ViewId);
		void Begin();
		void DrawQuad(glm::vec3 position, float rotation, uint32_t color, glm::vec3 scale, uint32_t depth);
		void DrawImage(Image* image, glm::vec3 position, float rotation, uint32_t color, uint32_t depth);
		void WriteToStencil();
		void EnableStencilTest();
		void DisableStencil();
		void SetPostProcessColor(uint32_t color);
		void SetClearColor2D(uint32_t color); //Make conversion from glm vec4 to uint32_t internally in function, todo
		void SetClearColor3D(uint32_t color);
		void Render();
		void Shutdown();

		

		//bgfx::ViewId GetMainViewId() const { return 0; }
		bgfx::RendererType::Enum GetRendererType() const { return bgfx::getRendererType(); }
		//bgfx::PlatformData GetPlatformData() const { return bgfx::getInternalData(); }
	private:

		// REMOVE STATIC DEFAULTS ONCE EACH CLASS THAT USES RENDERER HAS ITS OWN INSTANCE OF THESE RESOURCES. RENDERER
		static ShaderProgram* m_DefaultShaderProgram;
		static uint32_t m_Stencil;
		static bgfx::UniformHandle m_Uniform;
		static Image* m_WhiteImage;

		static bgfx::IndexBufferHandle m_IndexBuffer;
		static bgfx::VertexLayout m_VertexLayout;
		static bgfx::DynamicVertexBufferHandle m_VertexBuffer;
		//bgfx::UniformHandle m_Uniform;
		//Image* m_WhiteImage;
		//ShaderProgram* m_ShaderProgram;
		static std::vector<Vertex2D> m_Vertices;
		std::vector<RenderBatch> m_RenderBatches;
		static FrameBuffer* m_FrameBuffer;

		static bgfx::DynamicVertexBufferHandle m_NDCVertexBuffer;
		static std::vector<Vertex2D> m_NDCVertices;
		//uint32_t m_Stencil;

		static glm::vec3 m_At;
		static glm::vec3 m_Camera;
		static glm::mat4 m_View;
		static glm::mat4 m_Projection;
		static glm::mat4 m_PerspectiveProjection;

		uint32_t ClearColor2d = 0x00FFFFFF;
		uint32_t ClearColor3d = 0x000000FF;

		bool bImGuiInitialized = false;
	};
}



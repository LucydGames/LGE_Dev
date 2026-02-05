#include "renderer.h"

#include <print>
#include <glm/gtc/type_ptr.hpp>

Renderer::Renderer()
{

}

void Renderer::Initialize(SDL_Window* window, bgfx::RendererType::Enum RendererApiType)
{
	Log::GetInstance().PrintInfo("Initializing Renderer...");
	bgfx::Init init;
	init.resolution.width = SCREEN_WIDTH;
	init.resolution.height = SCREEN_HEIGHT;
	init.type = RendererApiType;
	init.vendorId = BGFX_PCI_ID_NONE;
	init.platformData.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);

	if (bgfx::init(init) == false)
	{
		Log::GetInstance().PrintError("Could not initialize BGFX");
	}
	

	std::vector<uint16_t> indices;

	for (uint32_t index = 0; index < 100; index++)
	{
		indices.push_back(4 * index + 3);
		indices.push_back(4 * index + 2);
		indices.push_back(4 * index + 0);

		indices.push_back(4 * index + 2);
		indices.push_back(4 * index + 1);
		indices.push_back(4 * index + 0);

	}

	m_IndexBuffer = bgfx::createIndexBuffer(bgfx::copy(indices.data(), indices.size() * 2));
	if (bgfx::isValid(m_IndexBuffer) == false)
	{
		Log::GetInstance().PrintError("Could not create index buffer");
	}


	m_VertexLayout.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();

	m_Vertices.resize(100);
	m_VertexBuffer = bgfx::createDynamicVertexBuffer(bgfx::makeRef(m_Vertices.data(), sizeof(Vertex) * m_Vertices.size()), m_VertexLayout);
	if (bgfx::isValid(m_VertexBuffer) == false)
	{
		Log::GetInstance().PrintError("Could not create vertex buffer");
	}

	m_Uniform = bgfx::createUniform("textureColor", bgfx::UniformType::Sampler);
	if (bgfx::isValid(m_Uniform) == false)
	{
		Log::GetInstance().PrintError("Could not create uniform");
	}

	uint32_t white = 0xffffffff;
	m_WhiteImage = new Image(&white, 1, 1);

	switch (bgfx::getRendererType())
	{
	case bgfx::RendererType::Vulkan:
		m_ShaderProgram = new ShaderProgram("../LGE/vertexspirv.bin", "../LGE/fragspirv.bin");
		break;
	case bgfx::RendererType::Direct3D11:
		m_ShaderProgram = new ShaderProgram("../LGE/vertexd3d.bin", "../LGE/fragd3d.bin");
		break;
	case bgfx::RendererType::Direct3D12:
		m_ShaderProgram = new ShaderProgram("../LGE/vertexd3d.bin", "../LGE/fragd3d.bin");
		break;
	case bgfx::RendererType::OpenGL:
		m_ShaderProgram = new ShaderProgram("../LGE/vertexspirv.bin", "../LGE/fragspirv.bin");
		break;
	default:
		m_ShaderProgram = new ShaderProgram("../LGE/vertexspirv.bin", "../LGE/fragspirv.bin");
		break;
	}
	//m_ShaderProgram = new ShaderProgram("../LGE/vertex.bin", "../LGE/frag.bin"); //Change path to sdk library default shaders 

	m_Camera = { 0.0f, 0.0f, -1.0f };
	m_At = { 0.0f, 0.0f, 0.0f };

	m_View = glm::lookAt(m_Camera, m_At, { 0.0f, 1.0f, 0.0f });
	m_Projection = glm::ortho(-SCREEN_WIDTH / 2.0f, SCREEN_WIDTH / 2.0f, -SCREEN_HEIGHT / 2.0f, SCREEN_HEIGHT / 2.0f, -1000.0f, 1000.0f);
	m_PerspectiveProjection = glm::perspective(glm::radians(60.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.01f, 100000.0f);


	m_FrameBuffer = new FrameBuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

	//                         Position              // Color   //Normal                // TexCoord/UV
	// m_NDCVertices.push_back({ {-1.0f, 1.0f, 0.0f}, 0xffffffff, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f} });     3d version.. Add support for both in one struct later
	m_NDCVertices.push_back({ {-1.0f, 1.0f, 0.0f}, 0xffffffff, {0.0f, 0.0f} });
	m_NDCVertices.push_back({ {1.0f, 1.0f, 0.0f}, 0xffffffff, {1.0f, 0.0f} });
	m_NDCVertices.push_back({ {1.0f, -1.0f, 0.0f}, 0xffffffff, {1.0f, 1.0f} });
	m_NDCVertices.push_back({ {-1.0f, -1.0f, 0.0f}, 0xffffffff, {0.0f, 1.0f} });
	m_NDCVertexBuffer = bgfx::createDynamicVertexBuffer(bgfx::makeRef(m_NDCVertices.data(), sizeof(Vertex) * m_NDCVertices.size()), m_VertexLayout);
}

void Renderer::InitializeImGui(int ViewId)
{
	if (bImGuiInitialized == false)
	{
		ImGui_ImplBgfx_Init(ViewId);
		bImGuiInitialized = true;
	}
}

void Renderer::Begin()
{
	bgfx::reset(SCREEN_WIDTH, SCREEN_HEIGHT, BGFX_RESET_VSYNC);
	m_Stencil = BGFX_STENCIL_NONE;

	m_Vertices.clear();
	m_RenderBatches.clear();

	bgfx::setViewClear(THREE_D_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, ClearColor3d);
	bgfx::setViewRect(THREE_D_VIEW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	bgfx::setViewTransform(THREE_D_VIEW, glm::value_ptr(m_View), glm::value_ptr(m_PerspectiveProjection));
	bgfx::setViewFrameBuffer(THREE_D_VIEW, m_FrameBuffer->GetFrameBufferHandle());

	bgfx::setViewRect(TWO_D_VIEW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	bgfx::setViewClear(TWO_D_VIEW, BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, ClearColor2d);
	bgfx::setViewTransform(TWO_D_VIEW, glm::value_ptr(m_View), glm::value_ptr(m_Projection));
	bgfx::setViewFrameBuffer(TWO_D_VIEW, m_FrameBuffer->GetFrameBufferHandle());

	bgfx::setViewRect(RENDER_VIEW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	bgfx::setViewClear(RENDER_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0xFFFFFFFF);

	bgfx::touch(TWO_D_VIEW);
	bgfx::touch(THREE_D_VIEW);

	if (bImGuiInitialized)
	{
		ImGui_ImplBgfx_NewFrame();
	}
}


void Renderer::DrawQuad(glm::vec3 position, float rotation, uint32_t color, glm::vec3 scale, uint32_t depth)
{
	RenderBatch batch;
	batch.Stencil = m_Stencil;
	batch.Depth = depth;
	batch.StartIndex = m_Vertices.size() / 4 * 6;
	batch.NumberOfIndices = 6;
	batch.Texture = m_WhiteImage->GetTextureHandle();
	batch.Transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::scale(glm::mat4(1.0f), scale);

	m_RenderBatches.push_back(batch);

	m_Vertices.push_back({ {-50.0f, 50.0f, 0.0f}, color, {0.0, 1.0f} });
	m_Vertices.push_back({ {50.0f, 50.0f, 0.0f}, color, {1.0, 1.0f} });
	m_Vertices.push_back({ {50.0f, -50.0f, 0.0f}, color, {1.0, 0.0f} });
	m_Vertices.push_back({ {-50.0f, -50.0f, 0.0f}, color, {0.0, 0.0f} });
}


void Renderer::DrawImage(Image* image, glm::vec3 position, float rotation, uint32_t color, uint32_t depth)
{
	RenderBatch batch;
	batch.Stencil = m_Stencil;
	batch.Depth = depth;
	batch.StartIndex = m_Vertices.size() / 4 * 6;
	batch.NumberOfIndices = 6;
	batch.Texture = image->GetTextureHandle();
	batch.Transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));

	m_RenderBatches.push_back(batch);

	m_Vertices.push_back({ {-50.0f, 50.0f, 0.0f}, color, {1.0, 1.0f} });
	m_Vertices.push_back({ {50.0f, 50.0f, 0.0f}, color, {0.0, 1.0f} });
	m_Vertices.push_back({ {50.0f, -50.0f, 0.0f}, color, {0.0, 0.0f} });
	m_Vertices.push_back({ {-50.0f, -50.0f, 0.0f}, color, {1.0, 0.0f} });
}

void Renderer::WriteToStencil()
{
	m_Stencil = BGFX_STENCIL_OP_FAIL_S_KEEP | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_REPLACE | BGFX_STENCIL_TEST_ALWAYS | BGFX_STENCIL_FUNC_REF(1) | BGFX_STENCIL_FUNC_RMASK(0xff);
}

void Renderer::EnableStencilTest()
{
	m_Stencil = BGFX_STENCIL_OP_FAIL_S_KEEP | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_REPLACE | BGFX_STENCIL_TEST_NOTEQUAL | BGFX_STENCIL_FUNC_REF(1) | BGFX_STENCIL_FUNC_RMASK(0xff);
}

void Renderer::DisableStencil()
{
	m_Stencil = BGFX_STENCIL_NONE;
}

void Renderer::SetPostProcessColor(uint32_t color)
{
	m_NDCVertices[0].Color = color;
	m_NDCVertices[1].Color = color;
	m_NDCVertices[2].Color = color;
	m_NDCVertices[3].Color = color;
	bgfx::update(m_NDCVertexBuffer, 0, bgfx::makeRef(m_NDCVertices.data(), m_NDCVertices.size() * sizeof(Vertex)));
}

void Renderer::SetClearColor2D(uint32_t color)
{
	ClearColor2d = color;
	//bgfx::setViewClear(TWO_D_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, ClearColor2d);
}

void Renderer::SetClearColor3D(uint32_t color)
{
	ClearColor3d = color;
	//bgfx::setViewClear(THREE_D_VIEW, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, ClearColor3d);
}

void Renderer::Render()
{
	if (m_Vertices.size() > 0)
	{
		bgfx::update(m_VertexBuffer, 0, bgfx::makeRef(m_Vertices.data(), m_Vertices.size() * sizeof(Vertex)));
	}
	bgfx::update(m_NDCVertexBuffer, 0, bgfx::makeRef(m_NDCVertices.data(), m_NDCVertices.size() * sizeof(Vertex)));

	for (RenderBatch& batch : m_RenderBatches)
	{
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_ALPHA | BGFX_STATE_WRITE_Z);
		bgfx::setStencil(batch.Stencil);
		bgfx::setTransform(glm::value_ptr(batch.Transform));
		bgfx::setVertexBuffer(0, m_VertexBuffer);
		bgfx::setIndexBuffer(m_IndexBuffer, batch.StartIndex, batch.NumberOfIndices);
		bgfx::setTexture(0, m_Uniform, batch.Texture);
		bgfx::submit(TWO_D_VIEW, m_ShaderProgram->GetProgramHandle(), batch.Depth);
	}

	bgfx::setVertexBuffer(0, m_NDCVertexBuffer);
	bgfx::setIndexBuffer(m_IndexBuffer, 0, 6);
	bgfx::setTexture(0, m_Uniform, m_FrameBuffer->GetColorAttachmentHandle());
	bgfx::submit(RENDER_VIEW, m_ShaderProgram->GetProgramHandle());

	if (bImGuiInitialized)
	{
		ImGui::Render();
		ImGui_ImplBgfx_RenderDrawData(ImGui::GetDrawData());
	}

	bgfx::frame();
}

void Renderer::Shutdown()
{
	bgfx::destroy(m_IndexBuffer);
	bgfx::destroy(m_VertexBuffer);
	delete m_ShaderProgram;
	delete m_WhiteImage;
	bgfx::destroy(m_Uniform);
	bgfx::shutdown();
}
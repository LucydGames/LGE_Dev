#pragma once
// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <bgfx/bgfx.h>

#include "renderer_common.h"
#include "meta.h"
#include "log.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// std
#include <string>
#include <memory>
#include <vector>

//TODO: Fix non initialized member variables have loadmodelfromfile call constructor and init buffers in constructor

namespace lge
{
	class LgeModel
	{
	public:

		struct Builder {
			std::vector<Vertex> vertices{};
			std::vector<uint16_t> indices{};

			void loadModel(const std::string& filepath);
		};

		LgeModel() {};
		~LgeModel() {};

		static std::unique_ptr<LgeModel> LoadModelFromFile(const std::string& filepath);
		void Draw(glm::vec3 position, float rotation, glm::vec3 scale, uint32_t stencil, bgfx::UniformHandle uniform, bgfx::TextureHandle textureHandle, bgfx::ProgramHandle shaderProgram);
		const std::vector<glm::vec3>& GetVertices() const { return m_Vertices; }
		const std::vector<uint16_t>& GetIndices() const { return m_Indices; }

		static std::unique_ptr<LgeModel> CreatePyramid();
		//void DrawPyramid(glm::vec3 position, float rotation, uint32_t stencil, bgfx::UniformHandle uniform, bgfx::TextureHandle textureHandle, bgfx::ProgramHandle shaderProgram);

	private:
		std::vector<glm::vec3> m_Vertices;
		std::vector<uint16_t> m_Indices;

		bgfx::IndexBufferHandle m_IndexBuffer;
		bgfx::VertexBufferHandle m_VertexBuffer;
		bgfx::VertexLayout m_VertexLayout;
	};
}


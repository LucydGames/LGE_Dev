#include "lge_model.h"

#define TINYOBJLOADER_IMPLEMENTATION

#include "tiny_obj_loader.h"
#include <cstring>
#include <stdexcept>
#include <cassert>

namespace lge
{
	void LgeModel::Builder::loadModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		for (const auto& shape : shapes) 
		{
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};
				vertex.Position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
				if (index.texcoord_index >= 0) {
					vertex.UV = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}
				vertices.push_back(vertex);
				indices.push_back(static_cast<uint32_t>(indices.size()));
			}
		}
	}

	std::unique_ptr<LgeModel> LgeModel::LoadModelFromFile(const std::string& filepath)
	{
		Builder builder;
		builder.loadModel(filepath); // Make define for Engine Directory then filepath will be <ENGINEDIR> /assets/models/... to shorten filepaths
		auto model = std::make_unique<LgeModel>();
		model->m_Vertices.reserve(builder.vertices.size());
		for (const auto& vertex : builder.vertices) {
			model->m_Vertices.push_back(vertex.Position);
		}
		model->m_Indices = std::move(builder.indices);
		model->m_IndexBuffer = bgfx::createIndexBuffer(bgfx::copy(model->m_Indices.data(), model->m_Indices.size() * sizeof(uint32_t)));
		model->m_VertexLayout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();
		model->m_VertexBuffer = bgfx::createVertexBuffer(bgfx::copy(builder.vertices.data(), builder.vertices.size() * sizeof(Vertex)), model->m_VertexLayout);
		Log::GetInstance().PrintInfo("Loaded model from file: " + filepath);
		return model;
	}

	void lge::LgeModel::Draw(glm::vec3 position, float rotation, uint32_t stencil, bgfx::UniformHandle uniform, bgfx::TextureHandle textureHandle, bgfx::ProgramHandle shaderProgram) // Shader Program should be set before calling this. Argument? Also Draw wont be handled by objects but by renderer/render system
	{

		glm::mat4 ModelMatrix = glm::mat4(1.0f); // Identity matrix for now
		glm::mat4 Transform = glm::translate(ModelMatrix, position) * glm::rotate(ModelMatrix, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 Scale = glm::vec3(0.005f, 0.005f, 0.005f); // No dynamic scaling for now
		//Transform = glm::scale(Transform, Scale);
		bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_BLEND_ALPHA, BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS);
		bgfx::setStencil(stencil);
		bgfx::setTransform(glm::value_ptr(Transform));
		bgfx::setVertexBuffer(0, m_VertexBuffer);
		bgfx::setIndexBuffer(m_IndexBuffer);
		bgfx::setTexture(0, uniform, textureHandle);
		bgfx::submit(THREE_D_VIEW, shaderProgram);
		//bgfx::submit(THREE_D_VIEW, nullptr);
	}
}
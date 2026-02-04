#include "lge_model.h"
#include "lge_utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cstring>
#include <stdexcept>
#include <cassert>
#include <unordered_map>

namespace std {
	template <>
	struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			size_t seed = 0;
			lge::hashCombine(seed, vertex.Position, vertex.Color, vertex.Normal, vertex.U, vertex.V);
			return seed;
		}
	};
}  // namespace std

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

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};
		for (const auto& shape : shapes) 
		{
			for (const auto& index : shape.mesh.indices) {
				Vertex vertex{};
				vertex.Position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};
				
				vertex.Color = 0xffffffff;
				// convert vec3 to uint32_t Using vertex colors
					//ConvertRGBToUInt32Color(attrib.colors[3 * index.vertex_index + 0], attrib.colors[3 * index.vertex_index + 1], attrib.colors[3 * index.vertex_index + 2])
					//Use Normals as color
					//EncodeRgbToUint32Color(attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2]);
					// Or generate random color
					//GenerateRandomUInt32_tColor()
				
				if (index.normal_index >= 0) {
					vertex.Normal = {
						encodeNormalRgba8(attrib.normals[3 * index.normal_index + 0], attrib.normals[3 * index.normal_index + 1], attrib.normals[3 * index.normal_index + 2])
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.U = {
						ScalarToSNorm16(attrib.texcoords[2 * index.texcoord_index + 0]) //try using int format like bgfx examples
					};
					vertex.V = {
						ScalarToSNorm16(1.0f - attrib.texcoords[2 * index.texcoord_index + 1])
					};
				}
				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(uniqueVertices[vertex]);
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
		model->m_IndexBuffer = bgfx::createIndexBuffer(bgfx::copy(model->m_Indices.data(), model->m_Indices.size() * sizeof(uint16_t))); // 
		model->m_VertexLayout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true) // Add normal at some point
			.end();
		//Fixed stupid AI auto complete bug, Vertext buffer needs actual vertex data taken from struct Vertex.Position, not full data of Vertex struct which contains garbage data not init (Normals, color, etc)
		//Vertex2D vertices[] = builder.vertices.data();
		model->m_VertexBuffer = bgfx::createVertexBuffer(bgfx::copy(builder.vertices.data(), builder.vertices.size() * sizeof(Vertex)), model->m_VertexLayout);
		if (bgfx::isValid(model->m_IndexBuffer) == false)
		{
			Log::GetInstance().PrintWarning("Could not create model index buffer from file: " + filepath);
		}
		else
		{
			Log::GetInstance().PrintInfo("Loaded model from file: " + filepath);
		}
		return model;
	}

	void lge::LgeModel::Draw(glm::vec3 position, float rotation, glm::vec3 scale, uint32_t stencil, bgfx::UniformHandle uniform, bgfx::TextureHandle textureHandle, bgfx::ProgramHandle shaderProgram) // Shader Program should be set before calling this. Argument? Also Draw wont be handled by objects but by renderer/render system
	{

		glm::mat4 ModelMatrix = glm::mat4(1.0f); // Identity matrix for now
		glm::mat4 Transform = glm::translate(ModelMatrix, position) * glm::rotate(ModelMatrix, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		//glm::vec3 Scale = glm::vec3(0.5f, 0.5f, 0.5f); // No dynamic scaling for now
		Transform = glm::scale(Transform, scale);
		bgfx::setState(0 | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA);
		bgfx::setStencil(stencil);
		bgfx::setTransform(glm::value_ptr(Transform));
		bgfx::setVertexBuffer(0, m_VertexBuffer);
		bgfx::setIndexBuffer(m_IndexBuffer);
		bgfx::setTexture(0, uniform, textureHandle);
		bgfx::submit(THREE_D_VIEW, shaderProgram);
		//bgfx::submit(THREE_D_VIEW, nullptr);
	}
	std::unique_ptr<LgeModel> LgeModel::CreatePyramid()
	{
		auto model = std::make_unique<LgeModel>();
		uint16_t pyramidIndices[] =
		{
			0,1,2,  // Front
			0,2,3,  // Right
			0,3,4,  // Back
			0,4,1,  // Left
			1,4,3,  // Bottom
			1,3,2,  // Bottom
		};
		model->m_IndexBuffer = bgfx::createIndexBuffer(
			bgfx::copy(pyramidIndices, sizeof(pyramidIndices) * 2)
		);

		if (bgfx::isValid(model->m_IndexBuffer) == false)
		{
			Log::GetInstance().PrintWarning("Could not create pyramic index buffer");
		}
		else
		{
			Log::GetInstance().PrintInfo("Pyramid model created");
		}
		model->m_VertexLayout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		Vertex2D pyramidVertices[] = {
		{ {  0.0f,  50.0f,  0.0f}, 0xff0000ff  , { 0.0f, 0.0f }},
		{ { -50.0f, -50.0f, 50.0f }, 0xff00ff00  , { 0.0f, 0.0f } },
		{ { 50.0f, -50.0f,  50.0f }, 0xffff0000  , { 0.0f, 0.0f } },
		{ { 50.0f, -50.0f, -50.0f }, 0xffffff00  , { 0.0f, 0.0f } },
		{ { -50.0f, -50.0f, -50.0f }, 0xff00ffff  , { 0.0f, 0.0f } }
		};

		model->m_VertexBuffer = bgfx::createVertexBuffer(bgfx::copy(pyramidVertices, sizeof(pyramidVertices)), model->m_VertexLayout);
		return model;
	}
	//void LgeModel::DrawPyramid(glm::vec3 position, float rotation, uint32_t stencil, bgfx::UniformHandle uniform, bgfx::TextureHandle textureHandle, bgfx::ProgramHandle shaderProgram)
	//{
	//}
}
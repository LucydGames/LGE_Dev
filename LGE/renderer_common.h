#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex
{
	glm::vec3 Position;
	uint32_t Color;
	glm::vec3 Normal{};
	glm::vec2 UV;

	bool operator==(const Vertex& other) const {
		return Position == other.Position && Color == other.Color && Normal == other.Normal &&
			UV == other.UV;
	}
};

struct Vertex2D
{
	glm::vec3 Position;
	uint32_t Color;
	glm::vec2 TextureCoordinates; // change to Uv for consistency fix in renderer.cpp
};
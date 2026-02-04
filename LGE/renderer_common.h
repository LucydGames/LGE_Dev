#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


struct Vertex
{
	glm::vec3 Position;
	uint32_t Color;
	uint32_t  Normal{};
	int16_t  U;
	int16_t V;

	bool operator==(const Vertex& other) const {
		return Position == other.Position && Color == other.Color && Normal == other.Normal &&
			U == other.U && V == other.V;
	}
};

struct Vertex2D
{
	glm::vec3 Position;
	uint32_t Color;
	glm::vec2 TextureCoordinates; // change to Uv for consistency fix in renderer.cpp
};
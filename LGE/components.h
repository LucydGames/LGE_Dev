#pragma once

#include "entt.hpp"
#include "glm/glm.hpp"

struct TransformComponent
{
	glm::mat4 Transform;

	TransformComponent() = default;
	TransformComponent(const TransformComponent&) = default;
	TransformComponent(const glm::mat4& transform) : Transform(transform) {}

	//Implicit cast to glm mat4
	operator const glm::mat4& () const { return Transform; }
	operator const glm::mat4& () { return Transform;  }

	glm::vec3& GetLocation()
	{
		return *reinterpret_cast<glm::vec3*>(&Transform[3]);
	}

	glm::vec3 GetRotation() const
	{
		glm::vec3 scale;
		scale.x = glm::length(glm::vec3(Transform[0]));
		scale.y = glm::length(glm::vec3(Transform[1]));
		scale.z = glm::length(glm::vec3(Transform[2]));
		glm::mat4 rotMat = Transform;
		rotMat[0] /= scale.x;
		rotMat[1] /= scale.y;
		rotMat[2] /= scale.z;
		float sy = -rotMat[2][0];
		float cy = sqrt(1 - sy * sy);
		glm::vec3 rotation;
		if (cy > 16 * std::numeric_limits<float>::epsilon())
		{
			rotation.x = atan2(rotMat[2][1], rotMat[2][2]);
			rotation.y = asin(sy);
			rotation.z = atan2(rotMat[1][0], rotMat[0][0]);
		}
		else
		{
			rotation.x = atan2(-rotMat[1][2], rotMat[1][1]);
			rotation.y = asin(sy);
			rotation.z = 0;
		}
		return rotation;
	}

};
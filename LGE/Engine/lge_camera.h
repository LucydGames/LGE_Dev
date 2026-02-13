#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "bgfx/bgfx.h"

namespace lge
{

	class LgeCamera
	{
		LgeCamera() {};
		~LgeCamera() {};
	public:

		static LgeCamera& GetInstance()
		{
			static LgeCamera instance;
			return instance;
		}
		void SetPosition(const glm::vec3& position) { m_Position = position; UpdateViewMatrix(); }
		void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; UpdateViewMatrix(); }
		void SetProjection(float fov, float aspectRatio, float nearPlane, float farPlane)
		{
			m_ProjectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
		}
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetRotation() const { return m_Rotation; }
	private:
		void UpdateViewMatrix()
		{
			//glm::mat4 rotationMatrix = glm::yawPitchRoll(glm::radians(m_Rotation.y), glm::radians(m_Rotation.x), glm::radians(m_Rotation.z));
			//glm::vec3 forward = glm::normalize(rotationMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
			//glm::vec3 up = glm::normalize(rotationMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
			//m_ViewMatrix = glm::lookAt(m_Position, m_Position + forward, up);
		}
		glm::vec3 m_Position{ 0.0f, 0.0f, 5.0f };
		glm::vec3 m_Rotation{ 0.0f, 0.0f, 0.0f };
		glm::mat4 m_ViewMatrix{ 1.0f };
		glm::mat4 m_ProjectionMatrix{ 1.0f };
	};

}
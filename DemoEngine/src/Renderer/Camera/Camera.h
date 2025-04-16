#pragma once

#include <glm/glm.hpp>

namespace DemoEngine 
{
	enum class ProjectionType { Orthographic = 0, Perspective = 1};

	class Camera 
	{
	public:

		Camera() = default;
		Camera(const glm::mat4& _projection) : m_Projection(_projection) {}
		virtual ~Camera() = default;

		const glm::mat4& GetProjection() const { return m_Projection; }

	protected:
		glm::mat4 m_Projection = glm::mat4(1.0f);
	};
}
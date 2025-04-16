#pragma once
#include <Glad/glad.h>

namespace DemoEngine 
{
	class Renderer3D 
	{
	public:
		Renderer3D() {};
		~Renderer3D() = default;

		void PreProcessing();

		void RenderTriange();
		void SetupTriangle();
		void UpdateViewportSize(int width, int height);

	private:
		uint32_t m_VertexArray, m_VertexBuffer, m_IndexBuffer;

	};
}
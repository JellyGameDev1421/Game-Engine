#pragma once
#include "Renderer/Data/VertexBuffer.h"

namespace DemoEngine 
{
	class OpenGLVertexBuffer : public VertexBuffer 
	{
	public: 
		OpenGLVertexBuffer(float* verticies, uint32_t size);
		OpenGLVertexBuffer(uint32_t size);

		virtual ~OpenGLVertexBuffer();
		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void SetData(const void* data, uint32_t size) override;
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};
}
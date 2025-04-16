#pragma once
#include "Renderer/Data/IndexBuffer.h"

namespace DemoEngine 
{
	class OpenGLIndexBuffer : public IndexBuffer 
	{
	public:
		OpenGLIndexBuffer(uint32_t* indicies, uint32_t size);
		virtual ~OpenGLIndexBuffer();
		virtual void Bind() const;
		virtual void UnBind() const;
		
		virtual uint32_t GetCount() const { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}
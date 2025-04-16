#include "DemoEngine_PCH.h"
#include "Buffer.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"

namespace DemoEngine 
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		return CreateRef<OpenGLVertexBuffer>(size);
	}

	Ref<VertexBuffer> VertexBuffer::Create(float* verticies, uint32_t size) 
	{
		return CreateRef<OpenGLVertexBuffer>(verticies, size);
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indicies, uint32_t size) 
	{
		return CreateRef<OpenGLIndexBuffer>(indicies, size);
	}
}
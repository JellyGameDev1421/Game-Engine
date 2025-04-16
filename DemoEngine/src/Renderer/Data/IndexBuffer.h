#pragma once
#include "DemoEngine_PCH.h"

namespace DemoEngine 
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual uint32_t GetCount() const = 0;

		static Ref<IndexBuffer> Create(uint32_t* indicies, uint32_t count);
	};
}
#include "DemoEngine_PCH.h"
#include "Framebuffer.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

#include <Glad/glad.h>

namespace DemoEngine 
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec) 
	{
		return CreateRef<OpenGLFramebuffer>(spec);
	}
}
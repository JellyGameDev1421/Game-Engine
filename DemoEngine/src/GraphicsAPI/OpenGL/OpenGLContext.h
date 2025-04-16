#pragma once
#include "Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace DemoEngine 
{
	class OpenGlContext : public GraphicsContext 
	{
	public:
		//Stores Glfwwindow in the pointer windowHandle
		OpenGlContext(GLFWwindow* windowHandle);

		//Override to indicate it overrides a virtual method from the graphics Context
		virtual void Init() override;

		//OpenGL normal double buffers and this ensures the rendered frame is shown on screen
		virtual void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}
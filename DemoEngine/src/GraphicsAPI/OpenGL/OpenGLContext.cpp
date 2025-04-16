#include "DemoEngine_PCH.h"
#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace DemoEngine
{
	OpenGlContext::OpenGlContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
	{

	}

	void OpenGlContext::Init() 
	{
		glfwMakeContextCurrent(m_WindowHandle);

		//Glad gets pointers to modern opengl function from the GPU
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CORE_ASSERT(status, "{0} - Failed to initialize Glad!", status);

		//print the driver we are using
		LOG_INFO("Vendor:			{0}", (const char*)glGetString(GL_VENDOR));
		LOG_INFO("Renderer:			{0}", (const char*)glGetString(GL_RENDERER));
		LOG_INFO("Version OpenGl:	{0}", (const char*)glGetString(GL_VERSION));
		LOG_INFO("Version GLSL:		{0}", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	void OpenGlContext::SwapBuffers() 
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}
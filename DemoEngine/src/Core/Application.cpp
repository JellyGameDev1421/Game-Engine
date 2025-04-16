#pragma once

#include "DemoEngine_PCH.h"
#include "Application.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Renderer/2D/Renderer2D.h"

/*Handling the main loop and window updates*/

namespace DemoEngine 
{
#define BIND_EVENT_FUNC(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application* Application::s_Instance = nullptr;

	//Sets up the default values
	Application::Application(const std::string& name) 
	{
		s_Instance = this;
		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		Input::Init();
		Input::SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer2D::Init();
		
		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	//Cleaning up after use Deconstructor
	Application::~Application () {}

	void Application::Close() 
	{
		m_Running = false;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FUNC(OnWindowResize));
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if (e.Handled) {
				break;
			}
			(*it)->OnEvent(e);
		}
	}

	//Main loop it using something similar to delta time
	void Application::Run() 
	{
		while (m_Running)
		{
			float time = (float)glfwGetTime(); //Platform get time
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimized)
			{

				{
					for (Layer* layer : m_LayerStack)
					{
						layer->OnUpdate(timestep);
					}
				}

				m_ImGuiLayer->Begin();
				{
					for (Layer* layer : m_LayerStack)
					{
						layer->OnImGuiRender();
					}
				}
				m_ImGuiLayer->End();

				Input::Update();

			}
			m_Window->OnUpdate();

		}

	}

	void Application::PushLayer (Layer* layer) 
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay (Layer* layer) 
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) 
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) 
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0) 
		{
			m_Minimized = true;
			return false;
		}
		m_Minimized = false;

		return false;
	}
}
#pragma once

#include "Core.h"
#include "Window.h"
#include "Core/Timestep.h"
#include "Layer/LayerStack.h"
#include "Events/Events.h"
#include "Events/ApplicationEvent.h"
#include "ImGui/ImGuiLayer.h"

//Main Declaring allowing it access with the Applications private functions
int main(int argc, char** argv);

namespace DemoEngine 
{
	class Application 
	{
	public:
		Application(const std::string& name = "DemoEngine");
		~Application();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline Window& GetWindow() { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

		static Application& Get() { return *s_Instance; }
	private:
		void Run();

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		float m_LastFrameTime = 0.0f;

		LayerStack m_LayerStack;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};
	//Defined in client
	Application* CreateApplication();
}
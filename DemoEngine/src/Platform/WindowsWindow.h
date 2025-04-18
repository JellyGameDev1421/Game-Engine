#pragma once
#include "Core/Window.h"
#include "GLFW/glfw3.h"
#include "Events/Events.h"

#include "Renderer/GraphicsContext.h"

namespace DemoEngine
{
	class WindowsWindow : public Window 
	{
	public: 
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();
		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		void SetVSync(bool enabled) override; // Remove '= 0'
		bool IsVSync() const;

		inline virtual void* GetNativeWindow() const { return m_Window; }

	private:

		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		GLFWwindow* m_Window;
		GraphicsContext* m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;

		inline void SetEventCallback(const EventCallbackFn& callback) override {
			m_Data.EventCallback = callback;
		}
	};
}
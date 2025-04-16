#pragma once
#include "Core/Core.h"
#include "KeyCodes.h"
#include "MouseCodes.h"
#include "Events/Events.h"
#include <glm/glm.hpp>

namespace DemoEngine
{
	class Input 
	{
	public:

		static void Init();
		static void SetEventCallback(const EventCallbackFn& callback);
		static void Update();

		static bool IsKeyPressed(KeyCode key);
		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};

}
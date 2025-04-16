#include "DemoEngine_PCH.h"
#include "DemoEngine.h"
#include "Editor/EditorLayer.h"

#include "Core/EntryPoint.h"

namespace DemoEngine 
{
	class DemoEngineApp : public DemoEngine::Application
	{
	public:
		DemoEngineApp() : Application("Demo Engine Editor") 
		{
			PushLayer(new EditorLayer());
		}

		~DemoEngineApp() {}
	};

	DemoEngine::Application* DemoEngine::CreateApplication() {
		return new DemoEngineApp();
	}
}
#pragma once
#include <Core/Core.h>

extern DemoEngine::Application* DemoEngine::CreateApplication();

extern "C" 
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 1;
	_declspec(dllexport) int AmdPowerExpressRequestHighPerformance = 1;
}


int main(int arc, char** argv) 
{
	DemoEngine::Log::Init();

	printf("Demo Engine\n");

	auto app = DemoEngine::CreateApplication();

	app->Run();

	delete app;
}
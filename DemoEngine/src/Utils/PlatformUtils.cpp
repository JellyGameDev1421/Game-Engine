#include "DemoEngine_PCH.h"
#include "PlatformUtils.h"

#include "Platform/Windows/WindowsPlatformUtils.h"

namespace DemoEngine 
{
	std::string FileDialogs::OpenFile(const char* filter) 
	{
		return WindowsFileDialogs::OpenFile(filter);
	}

	std::string FileDialogs::SaveFile(const char* filter) 
	{
		return WindowsFileDialogs::SaveFile(filter);
	}
}
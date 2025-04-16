project "Enet"
	kind "staticlib"
	language "C"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	targetdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	
	files
	{
		"include/**.h",
		"src/**.c"
	}
	
	includedirs
	{
		"include/"
	}
	
	links {"ws2_32", "winmm"}
	
	filter "system:windows"
		buildoptions {"-std=c11"}
		systemversion "latest"
		staticruntime "On"
		
		files
		{
			"src/win32.c"
		}
		
		defines
		{
			"ws2_32"
		}
		
		filter {"system:windows" , "configurations:Release"}
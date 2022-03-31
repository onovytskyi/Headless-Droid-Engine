solution "HeadlessDroidEngine"
	location "../"
	configurations { "Debug", "Development", "Profile", "Final" }
	platforms { "Win64" }
    startproject "Game"
	systemversion "10.0.22000.0"
	
	filter { "platforms:Win64" }
		system "Windows"
		architecture "x64"
		defines { "HD_PLATFORM_WIN64", "WIN32", "_HAS_ITERATOR_DEBUGGING=0", "_CRT_NO_VA_START_VALIDATION" }
		linkoptions { "/ENTRY:mainCRTStartup" }
		
	filter { "configurations:Debug" }
		targetsuffix "-Debug"
		defines { "DEBUG", "HD_BUILD_DEBUG" }
		optimize "Off"
                characterset "Unicode"
                symbols "On"
		
	filter { "configurations:Development" }
		targetsuffix "-Development"
		defines { "HD_BUILD_DEVELOPMENT" }
		optimize "Speed"
                characterset "Unicode"
                symbols "On"
		
	filter { "configurations:Profile" }
		targetsuffix "-Profile"
		defines { "NDEBUG", "N_DEBUG", "HD_BUILD_PROFILE" }
		optimize "On"
                characterset "Unicode"
                symbols "On"
		flags { "FatalWarnings", "LinkTimeOptimization" }
		
	filter { "configurations:Final" }
		targetsuffix "-Final"
		defines { "NDEBUG", "N_DEBUG", "HD_BUILD_FINAL" }
		optimize "Full"
                characterset "Unicode"
		flags { "FatalWarnings", "LinkTimeOptimization" }
		
	filter { "files:**.hlsl" }
		buildaction "None"
	
	filter { "files:**.hlsli" }
		buildaction "None"
		
	filter { "files:**.md" }
		buildaction "None"
		
	filter { "files:**.ixx" }
		buildaction "ClCompile"
	
project "Externals"
	location "../Source/Externals"
	objdir "../Build/Externals/%{cfg.platform}%{cfg.buildcfg}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Lib"
	includedirs { "$(SolutionDir)/Source/" }
	files { "../Source/Externals/**.h", "../Source/Externals/**.hpp", "../Source/Externals/**.inl", "../Source/Externals/**.cpp", "../Source/Externals/**.asm" }
	buildoptions { "/bigobj" }
	
project "Engine"
	location "../Source/Engine"
	objdir "../Build/Engine/%{cfg.platform}%{cfg.buildcfg}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Lib"
	includedirs { "$(SolutionDir)/Source/", "$(SolutionDir)/packages/Microsoft.Direct3D.D3D12.1.4.9/build/native/include/" }
	files { "../Source/Engine/**.h", "../Source/Engine/**.cpp", "../Source/Engine/**.hpp" }
	removefiles { "SystemWindowLinux.cpp" }
	vpaths {
	   ["00_Config/*"] = {"../Source/Engine/Config/**.h", "../Source/Engine/Config/**.hpp", "../Source/Engine/Config/**.cpp" },
	   ["01_Foundation/*"] = {"../Source/Engine/Foundation/**.h", "../Source/Engine/Foundation/**.hpp", "../Source/Engine/Foundation/**.cpp" },
	   ["02_Debug/*"] = {"../Source/Engine/Debug/**.h", "../Source/Engine/Debug/**.hpp", "../Source/Engine/Debug/**.cpp" },
	   ["03_Framework/*"] = {"../Source/Engine/Framework/**.h", "../Source/Engine/Framework/**.hpp", "../Source/Engine/Framework/**.cpp" },
	   ["04_Engine/*"] = {"../Source/Engine/Engine/**.h", "../Source/Engine/Engine/**.hpp", "../Source/Engine/Engine/**.cpp" },
	}
	pchheader ("Engine/Config/Bootstrap.h")
	pchsource ("../Source/Engine/Config/Bootstrap.cpp")
	buildoptions { "/bigobj" }
	defines { "HD_ENGINE_PROJECT" }
	nuget { "Microsoft.Direct3D.D3D12:1.4.9" }
	
project "Game"
	location "../Source/Game"
	objdir "../Build/Game/%{cfg.platform}%{cfg.buildcfg}"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Bin"
    debugdir "../Bin"
	includedirs { "$(SolutionDir)/Source/" }
	libdirs { "../Lib/", }
	links { "Externals", "Engine" }
	files { "../Source/Game/**.h", "../Source/Game/**.hpp", "../Source/Game/**.cpp", "../Source/Shaders/**.hlsl", "../Source/Shaders/**.hlsli" }
	vpaths {
	   ["Shaders/*"] = {"../Source/Shaders/**.hlsl", "../Source/Shaders/**.hlsli"}
	}
	pchheader ("Game/Bootstrap.h")
	pchsource ("../Source/Game/Bootstrap.cpp")
    buildoptions { "/Zm256" }
	kind "WindowedApp"
		
project "Test"
	location "../Source/Test"
	objdir "../Build/Test/%{cfg.platform}%{cfg.buildcfg}"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Bin"
    debugdir "../Bin"
	includedirs { "$(SolutionDir)/Source/" }
	libdirs { "../Lib/" }
	links { "Externals", "Engine" }
	files { "../Source/Test/**.h", "../Source/Test/**.hpp", "../Source/Test/**.cpp" }
    buildoptions { "/Zm256" }
	kind "ConsoleApp"
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
	
project "HeadlessDroid"
	location "../Source/HeadlessDroid"
	objdir "../Build/HeadlessDroid/%{cfg.platform}%{cfg.buildcfg}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Lib"
	includedirs { "$(SolutionDir)/Source/", "$(SolutionDir)/Source/HeadlessDroid/", "$(SolutionDir)/packages/Microsoft.Direct3D.D3D12.1.4.9/build/native/include/" }
	files { "../Source/HeadlessDroid/**.h", "../Source/HeadlessDroid/**.cpp", "../Source/HeadlessDroid/**.hpp" }
	removefiles { "SystemWindowLinux.cpp" }
	vpaths {
	   ["00_Config/*"] = {"../Source/HeadlessDroid/Config/**.h", "../Source/HeadlessDroid/Config/**.hpp", "../Source/HeadlessDroid/Config/**.cpp" },
	   ["01_Foundation/*"] = {"../Source/HeadlessDroid/Foundation/**.h", "../Source/HeadlessDroid/Foundation/**.hpp", "../Source/HeadlessDroid/Foundation/**.cpp" },
	   ["02_Debug/*"] = {"../Source/HeadlessDroid/Debug/**.h", "../Source/HeadlessDroid/Debug/**.hpp", "../Source/HeadlessDroid/Debug/**.cpp" },
	   ["03_Framework/*"] = {"../Source/HeadlessDroid/Framework/**.h", "../Source/HeadlessDroid/Framework/**.hpp", "../Source/HeadlessDroid/Framework/**.cpp" },
	   ["04_Engine/*"] = {"../Source/HeadlessDroid/Engine/**.h", "../Source/HeadlessDroid/Engine/**.hpp", "../Source/HeadlessDroid/Engine/**.cpp" },
	}
	pchheader ("Config/Bootstrap.h")
	pchsource ("../Source/HeadlessDroid/Config/Bootstrap.cpp")
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
	includedirs { "$(SolutionDir)/Source/", "$(SolutionDir)/Source/HeadlessDroid/" }
	libdirs { "../Lib/", }
	links { "Externals", "HeadlessDroid" }
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
	links { "Externals", "HeadlessDroid" }
	files { "../Source/Test/**.h", "../Source/Test/**.hpp", "../Source/Test/**.cpp" }
    buildoptions { "/Zm256" }
	kind "ConsoleApp"
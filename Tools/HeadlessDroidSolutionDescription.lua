solution "HeadlessDroidEngine"
	location "../Development"
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
	location "../Development/Source/Externals"
	objdir "../Development/Build/Externals/%{cfg.platform}%{cfg.buildcfg}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Development/Lib"
	includedirs { "$(SolutionDir)/Source/" }
	files { "../Development/Source/Externals/**.h", "../Development/Source/Externals/**.hpp", "../Development/Source/Externals/**.inl", "../Development/Source/Externals/**.cpp", "../Development/Source/Externals/**.asm" }
	buildoptions { "/bigobj" }
	
project "Engine"
	location "../Development/Source/Engine"
	objdir "../Development/Build/Engine/%{cfg.platform}%{cfg.buildcfg}"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Development/Lib"
	includedirs { "$(SolutionDir)/Source/", "$(SolutionDir)/packages/Microsoft.Direct3D.D3D12.1.4.9/build/native/include/" }
	files { "../Development/Source/Engine/**.h", "../Development/Source/Engine/**.cpp", "../Development/Source/Engine/**.hpp" }
	removefiles { "SystemWindowLinux.cpp" }
	vpaths {
	   ["00_Config/*"] = {"../Development/Source/Engine/Config/**.h", "../Development/Source/Engine/Config/**.hpp", "../Development/Source/Engine/Config/**.cpp" },
	   ["01_Foundation/*"] = {"../Development/Source/Engine/Foundation/**.h", "../Development/Source/Engine/Foundation/**.hpp", "../Development/Source/Engine/Foundation/**.cpp" },
	   ["02_Debug/*"] = {"../Development/Source/Engine/Debug/**.h", "../Development/Source/Engine/Debug/**.hpp", "../Development/Source/Engine/Debug/**.cpp" },
	   ["03_Framework/*"] = {"../Development/Source/Engine/Framework/**.h", "../Development/Source/Engine/Framework/**.hpp", "../Development/Source/Engine/Framework/**.cpp" },
	   ["04_Engine/*"] = {"../Development/Source/Engine/Engine/**.h", "../Development/Source/Engine/Engine/**.hpp", "../Development/Source/Engine/Engine/**.cpp" },
	}
	pchheader ("Engine/Config/Bootstrap.h")
	pchsource ("../Development/Source/Engine/Config/Bootstrap.cpp")
	buildoptions { "/bigobj" }
	defines { "HD_ENGINE_PROJECT" }
	nuget { "Microsoft.Direct3D.D3D12:1.4.9" }
	
project "Game"
	location "../Development/Source/Game"
	objdir "../Development/Build/Game/%{cfg.platform}%{cfg.buildcfg}"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Bin"
    debugdir "../Bin"
	includedirs { "$(SolutionDir)/Source/" }
	libdirs { "../Development/Lib/", }
	links { "Externals", "Engine" }
	files { "../Development/Source/Game/**.h", "../Development/Source/Game/**.hpp", "../Development/Source/Game/**.cpp", "../Development/Source/Shaders/**.hlsl", "../Development/Source/Shaders/**.hlsli" }
	vpaths {
	   ["Shaders/*"] = {"../Development/Source/Shaders/**.hlsl", "../Development/Source/Shaders/**.hlsli"}
	}
	pchheader ("Game/Bootstrap.h")
	pchsource ("../Development/Source/Game/Bootstrap.cpp")
    buildoptions { "/Zm256" }
	kind "WindowedApp"
		
project "Test"
	location "../Development/Source/Test"
	objdir "../Development/Build/Test/%{cfg.platform}%{cfg.buildcfg}"
	language "C++"
	cppdialect "C++latest"
	targetdir "../Bin"
    debugdir "../Bin"
	includedirs { "$(SolutionDir)/Source/" }
	libdirs { "../Development/Lib/" }
	links { "Externals", "Engine" }
	files { "../Development/Source/Test/**.h", "../Development/Source/Test/**.hpp", "../Development/Source/Test/**.cpp" }
    buildoptions { "/Zm256" }
	kind "ConsoleApp"
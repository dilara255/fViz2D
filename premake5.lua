--should change AS_ prefix on defines later (after compiling an testing)

--SOLUTION: fViz2D_base
workspace "fViz2D_base"
	startproject "TestApp"

	configurations {"Debug", "Release"}
	
	platforms { "x86_64" }

	filter "platforms:x86_64"
		architecture "x86_64"
		defines "SYS_ARCH=x86_64"

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	filter {}
	
	defines 'COMPILE_CFG="%{cfg.buildcfg}"'
	defines 'CURR_ARCHTECTURE="%{cfg.architecture}"'
	defines 'CURR_SYSTEM="%{cfg.system}"'	
	
	IncludeDir = {}
	IncludeDir["GLFW3"] = "%{wks.location}/depend/glfw3_x64"
	IncludeDir["IMGUI"]  = "%{wks.location}/depend/imGui-docking"
	IncludeDir["STB"]  = "%{wks.location}/depend/stb"
	IncludeDir["SPDLOG"]  = "%{wks.location}/depend/spdlog/include"
	IncludeDir["F_AUX_API"] = "%{wks.location}/fAux/API"
	IncludeDir["F_VIZ2D_API"] = "%{wks.location}/fViz2D/API"

	LibDir = {}
	LibDir["F_AUX"]   = ("%{wks.location}/fAux/lib/" .. outputdir)
	LibDir["F_VIZ2D"]   = ("%{wks.location}/fViz2D/lib/" .. outputdir)
	LibDir["GLFW3"]   = ("%{wks.location}/depend/glfw3_x64/GLFW/lib/")

	binDir = "bin/" .. outputdir .. "/%{prj.name}"
	binIntDir = "bin-int/" .. outputdir .. "/%{prj.name}"
	
--PROJECT: fAux
project "fAux"
	location "fAux"
	kind "Staticlib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	pchheader "miscStdHeaders.h"
	pchsource "%{prj.name}/src/miscStdHeaders.cpp"

	defines "F_AUX"

	flags
	{
		"MultiProcessorCompile"
	}

	targetdir (binDir)
	objdir (binIntDir)

	files
	{
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/include/**.hpp",
		"%{prj.name}/include/**.h",
		"%{prj.name}/API/**.h",		
		"%{prj.name}/API/**.hpp"
	}

	includedirs {
		"%{IncludeDir.SPDLOG}",
		"%{prj.name}/include",
		"%{IncludeDir.F_AUX_API}"
	}

	filter "architecture:x86_64"

		defines "X64"

	filter "system:windows"
		systemversion "latest"
		--buildoptions "/MT" --may cause override, should do inside filter

		defines{ 
			"AS_PLATFORM_WINDOWS",
			"AS_BUILD_LIB"
		}

	filter "configurations:Debug"
		defines "AS_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines	"AS_RELEASE"
		optimize "on" 		

	filter {}
	
	postbuildcommands{
		("{COPY} %{cfg.buildtarget.relpath} %{LibDir.F_AUX}")
	}

--PROJECT: fViz2D
project "fViz2D"
	location "fViz2D"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	dependson {"fAux"}

	defines "F_VIZ2D"

	flags
	{
		"MultiProcessorCompile"
	}

	links ("OpenGL32.lib") 
	links ("%{LibDir.GLFW3}/glfw3.lib")
	links ("%{LibDir.F_AUX}/fAux.lib")

	targetdir (binDir)
	objdir (binIntDir)
	
	files
	{
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/include/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/include/**.hpp",
		"%{prj.name}/API/**.hpp",
		"%{prj.name}/API/**.h"
	}

	includedirs
	{
		"%{IncludeDir.SPDLOG}",
		"%{IncludeDir.IMGUI}",
		"%{IncludeDir.GLFW3}",
		"%{IncludeDir.STB}",
		"%{IncludeDir.F_AUX_API}",
		"%{IncludeDir.F_VIZ2D_API}",
		"%{prj.name}/include"	
	}

	filter "system:windows"
		systemversion "latest"
		--buildoptions "/MD" --may cause override, should do inside filter

		defines {
			"AS_PLATFORM_WINDOWS",
			"AS_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "AS_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines	"AS_RELEASE"
		optimize "on" 
	
	filter {}
	
	postbuildcommands{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/TestApp"),
		("{COPYFILE} ../" .. binDir .."/fViz2D.lib %{LibDir.F_VIZ2D}")
	}

--PROJECT: testApp
project "TestApp"
	location "TestApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	dependson {"fAux", "fViz2D"}

	defines "F_TESTAPP"

	flags
	{
		"MultiProcessorCompile"
	}

	links ("%{LibDir.F_AUX}/fAux.lib")
	links ("%{LibDir.F_VIZ2D}/fViz2D.lib")

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/include/**.h",
		"%{prj.name}/include/**.hpp"
	}

	includedirs
	{
		"%{prj.name}/include",
		"%{IncludeDir.SPDLOG}", --was this needed?
		"%{IncludeDir.F_AUX_API}",
		"%{IncludeDir.F_VIZ2D_API}"
	}

	filter "architecture:x86_64"
		defines "X64"

	filter "system:windows"
		systemversion "latest"
		--buildoptions "/MT" --may cause override, should do inside filter

		defines{
			"AS_PLATFORM_WINDOWS",
			"AS_BUILD_APP"
		}

	filter "configurations:Debug"
		defines "AS_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines	"AS_RELEASE"
		optimize "on" 		
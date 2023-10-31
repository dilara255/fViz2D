-- TODO: Should change AS_ prefix on defines later (after compiling an testing)
-- TODO: Review which defines are actually needed
-- TODO: For windows, is there another openGL lib (ie, for 64 bits)?
-- TODO: review if/where SPDLOG needs to be included
-- TODO: on windows, shouldn't try to copy to linSharedLibDest_x86_64

--SOLUTION: fViz2D_port
workspace "fViz2D_port"

	-- Choose bellow the library kinds to be compiled:
	fAuxLibraryKind = "Staticlib"
	--fAuxLibraryKind = "SharedLib"
	fViz2DlibraryKind = "Staticlib"
	--fViz2DlibraryKind = "SharedLib"	

	startproject "TestApp"

	toolset("clang")
	flags { "MultiProcessorCompile", "Verbose" }
	warnings "Extra"

	configurations {"Debug", "Release"}

	platforms { "x86_64" }

	filter "platforms:x86_64"
		architecture "x86_64"
		defines "SYS_ARCH=x86_64"
	filter {}

	defines { "F_V2_API=", "F_AUX_API=", "F_CLIENTAPP" }
	-- F_CLIENTAPP will be undefined on Aux and Viz, so any client projects have it by default
	-- Note that this will have to be changed when the logging system is made more general
	-- WARNING: right now, there's a danger of concurrency if there are multiple Client Apps and
	-- extra loggers aren't manually added (which needs a recompile of fAux)
	
	filter "system:windows"
		defines "AS_PLATFORM_WINDOWS"
		if fViz2DlibraryKind == "SharedLib"	then 
			undefines "F_V2_API"
			defines { "F_V2_API=__declspec(dllimport)" }
		end if fAuxLibraryKind == "SharedLib" then	
			undefines "F_AUX_API"
			defines { "F_AUX_API=__declspec(dllimport)" }
		end
	filter "system:linux"
		defines "F_OS_LINUX"
		linSharedLibDest_x86_64 = "/usr/local/lib64/"
	filter {}

	cfgDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	guiCfgRelPath = "../res/guiCfg/imgui.ini"

	binDir = "bin/" .. cfgDir .. "/%{prj.name}"
	binIntDir = "bin-int/" .. cfgDir .. "/%{prj.name}"

	IncludeDir = {}
	IncludeDir["GLFW3"]       = "%{wks.location}/depend/glfw3_x64"
	IncludeDir["IMGUI"]       = "%{wks.location}/depend/imGui-docking"
	IncludeDir["STB"]         = "%{wks.location}/depend/stb"
	IncludeDir["SPDLOG"]      = "%{wks.location}/depend/spdlog/include"
	IncludeDir["F_VIZ2D_API"] = "%{wks.location}/fViz2D/API"
	IncludeDir["F_AUX_API"]   = "%{wks.location}/fAux/API"

	LibDir = {}
	LibDir["F_AUX"]   = ("%{wks.location}/fAux/lib/" .. cfgDir)
	LibDir["F_VIZ2D"] = ("%{wks.location}/fViz2D/lib/" .. cfgDir)
	LibDir["GLFW3"]   = ("%{wks.location}/depend/glfw3_x64/GLFW/lib/")

--PROJECT: fAux
project "fAux"
	location "fAux"
	kind (fAuxLibraryKind)
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	pic "on"

	pchheader "miscStdHeaders.h"
	pchsource "%{prj.name}/src/miscStdHeaders.cpp"

	undefines "F_CLIENTAPP"
	defines "F_AUX"
	defines "AS_BUILD_LIB"

	targetdir (binDir)
	objdir (binIntDir)

	files
	{
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/include/**.hpp",
		"%{prj.name}/include/**.h",
		"%{prj.name}/APIs/**.h",
		"%{prj.name}/APIs/**.hpp"
	}

	includedirs {
		"%{IncludeDir.SPDLOG}",
		"%{prj.name}/include",
		"%{IncludeDir.F_AUX_API}"
	}

	filter "architecture:x86_64"
		defines "X64"
	filter {}

	filter "system:windows"
		systemversion "latest"
		--buildoptions "/MT" --may cause override, should do inside filter
		if fAuxLibraryKind == "SharedLib" then
			undefines "F_AUX_API"
			defines { "F_AUX_API=__declspec(dllexport)" }
		end
	filter {}

	filter "configurations:Debug"
		defines "AS_DEBUG"
		symbols "on"
	filter "configurations:Release"
		defines	"AS_RELEASE"
		optimize "on"
	filter {}

	postbuildcommands{ ("{COPYDIR} ../" .. binDir .. " %{LibDir.F_AUX}") }

--PROJECT: fViz2D
project "fViz2D"
	location "fViz2D"
	kind (fViz2DlibraryKind)
	language "C++"
	cppdialect "C++17"
	staticruntime "off"
	pic "on"

	undefines "F_CLIENTAPP"
	defines "F_VIZ2D"

	filter "system:linux"
		links ("OpenGL")
	filter "system:windows"
		links ("OpenGL32")
	filter {}
	
	filter{}
	links ("%{LibDir.GLFW3}/glfw3")
	links ("fAux")

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

	filter "architecture:x86_64"
		defines "X64"
	filter {}

	filter "system:windows"
		systemversion "latest"
		--buildoptions "/MT" --may cause override, should do inside filter
		if fViz2DlibraryKind == "SharedLib"	then
			undefines "F_V2_API"
			defines { "F_V2_API=__declspec(dllexport)" }
		end
	filter {}

	filter "configurations:Debug"
		defines "AS_DEBUG"
		symbols "on"
	filter "configurations:Release"
		defines	"AS_RELEASE"
		optimize "on"
	filter {}

	postbuildcommands{
		("{COPYDIR} ../" .. binDir .. " %{LibDir.F_VIZ2D}"),
		("{MKDIR} ../bin/" .. cfgDir .. "/TestApp"),
		("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. cfgDir .."/TestApp"),
	}
	
	filter "system:linux"
		postbuildcommands{ ("{COPYDIR} ../" .. binDir .. " " .. linSharedLibDest_x86_64) }
	filter {}

--PROJECT: testApp
project "TestApp"
	location "TestApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	defines "F_TESTAPP"
	defines "AS_BUILD_APP"

	links ("fAux")
	links ("fViz2D")

	targetdir (binDir)
	objdir (binIntDir)

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
		"%{IncludeDir.SPDLOG}", --is this really needed?
		"%{IncludeDir.F_AUX_API}",
		"%{IncludeDir.F_VIZ2D_API}"
	}

	filter "architecture:x86_64"
		defines "X64"
	filter {}

	filter "system:windows"
		systemversion "latest"
		--buildoptions "/MT" --may cause override, should do inside filter
	filter {}

	filter "configurations:Debug"
		defines "AS_DEBUG"
		symbols "on"
	filter "configurations:Release"
		defines	"AS_RELEASE"
		optimize "on"
	filter {}

	postbuildcommands{ ("{COPYFILE} " .. guiCfgRelPath .. " ../bin/" .. cfgDir .."/TestApp/") }

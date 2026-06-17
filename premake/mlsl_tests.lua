local mlslRoot = path.getabsolute(path.join(_SCRIPT_DIR, ".."))

project "mlsl_tests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++23"
	characterset "Unicode"
	staticruntime "Off"

	filter "system:windows"
		systemversion "latest"

	filter {}

	targetdir "Build/%{cfg.buildcfg}/Output"
	objdir "Build/%{cfg.buildcfg}/Intermediates/%{prj.name}"

	files
	{
		path.join(mlslRoot, "Tests/mlsl_tests/**.cpp")
	}

	includedirs
	{
		path.join(mlslRoot, "mlsl/include")
	}

	links
	{
		"mlsl"
	}

	filter "configurations:Debug"
		defines
		{
			"_DEBUG"
		}
		symbols "On"

	filter "configurations:Release"
		defines
		{
			"NDEBUG"
		}
		optimize "On"

	filter {}

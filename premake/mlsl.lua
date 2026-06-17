local mlslRoot = path.getabsolute(path.join(_SCRIPT_DIR, ".."))

project "mlsl"
	kind "StaticLib"
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
		path.join(mlslRoot, "mlsl/include/**.hpp"),
		path.join(mlslRoot, "mlsl/include/**.inl"),
		path.join(mlslRoot, "mlsl/src/**.cpp")
	}

	includedirs
	{
		path.join(mlslRoot, "mlsl/include")
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
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

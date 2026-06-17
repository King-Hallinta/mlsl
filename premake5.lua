-- mlsl
-- SPDX-FileCopyrightText: 2026 Korrikada
-- SPDX-License-Identifier: Apache-2.0

workspace "mlsl"
	location "Build/ProjectFiles"
	startproject "mlsl_tests"
	architecture "x64"
	configurations { "Debug", "Release" }
	platforms { "x64" }

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
		"mlsl/include/**.hpp",
		"mlsl/include/**.inl",
		"mlsl/src/**.cpp"
	}

	includedirs
	{
		"mlsl/include"
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
		"Tests/mlsl_tests/**.cpp"
	}

	includedirs
	{
		"mlsl/include"
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

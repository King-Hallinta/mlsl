-- mlsl
-- SPDX-FileCopyrightText: 2026 Korrikada
-- SPDX-License-Identifier: Apache-2.0

workspace "mlsl"
	location "Build/ProjectFiles"
	startproject "mlsl_tests"
	architecture "x64"
	configurations { "Debug", "Release" }
	platforms { "x64" }

include "premake/mlsl.lua"
include "premake/mlsl_tests.lua"

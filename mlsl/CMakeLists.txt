# mlsl
# SPDX-FileCopyrightText: 2026 Korrikada
# SPDX-License-Identifier: Apache-2.0

file(GLOB_RECURSE MLSL_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

mlsl_static_library(mlsl 23
	SOURCES ${MLSL_SOURCES}
	INCLUDES
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
	DEFINES
		_CRT_SECURE_NO_WARNINGS)

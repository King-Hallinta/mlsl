# mlsl
# SPDX-FileCopyrightText: 2026 Korrikada
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.25)

project(mlsl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(MLSL_BUILD_TESTS "Build the mlsl tests" ON)

include(cmake/MlslTargets.cmake)

add_subdirectory(mlsl)

if(MLSL_BUILD_TESTS)
	include(cmake/VendorTargets.cmake)
	enable_testing()
	add_subdirectory(Tests/mlsl_tests)
	set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT mlsl_tests)
endif()

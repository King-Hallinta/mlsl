# mlsl
# SPDX-FileCopyrightText: 2026 Korrikada
# SPDX-License-Identifier: Apache-2.0

set(catch2_source_dir "${PROJECT_SOURCE_DIR}/vendor/Catch2")

if(NOT EXISTS "${catch2_source_dir}/CMakeLists.txt")
	message(FATAL_ERROR "Catch2 submodule is missing. Run: git submodule update --init --recursive")
endif()

add_subdirectory("${catch2_source_dir}" "${CMAKE_BINARY_DIR}/vendor/Catch2")

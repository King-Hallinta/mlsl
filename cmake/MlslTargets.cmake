# mlsl
# SPDX-FileCopyrightText: 2026 Korrikada
# SPDX-License-Identifier: Apache-2.0

function(mlsl_configure_target target standard)
	set_target_properties(${target} PROPERTIES
		CXX_STANDARD ${standard}
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/Output"
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/Output"
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIG>/Output")

	if(MSVC)
		set_target_properties(${target} PROPERTIES
			MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	endif()

	target_compile_definitions(${target} PRIVATE
		$<$<CONFIG:Debug>:_DEBUG>
		$<$<CONFIG:Release>:NDEBUG>)

	if(WIN32)
		target_compile_definitions(${target} PRIVATE
			UNICODE
			_UNICODE)
	endif()
endfunction()

function(mlsl_static_library target standard)
	cmake_parse_arguments(ARG "" "" "SOURCES;INCLUDES;LINKS;DEFINES" ${ARGN})

	if(NOT ARG_SOURCES)
		set(generated_source "${CMAKE_BINARY_DIR}/Generated/${target}.cpp")
		file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/Generated")
		file(WRITE "${generated_source}" "\n")
		list(APPEND ARG_SOURCES "${generated_source}")
	endif()

	add_library(${target} STATIC ${ARG_SOURCES})
	mlsl_configure_target(${target} ${standard})

	if(ARG_INCLUDES)
		target_include_directories(${target} PUBLIC ${ARG_INCLUDES})
	endif()

	if(ARG_LINKS)
		target_link_libraries(${target} PUBLIC ${ARG_LINKS})
	endif()

	if(ARG_DEFINES)
		target_compile_definitions(${target} PRIVATE ${ARG_DEFINES})
	endif()
endfunction()

function(mlsl_console_application target standard)
	cmake_parse_arguments(ARG "" "" "SOURCES;INCLUDES;LINKS;DEFINES" ${ARGN})

	add_executable(${target} ${ARG_SOURCES})
	mlsl_configure_target(${target} ${standard})

	if(ARG_INCLUDES)
		target_include_directories(${target} PRIVATE ${ARG_INCLUDES})
	endif()

	if(ARG_LINKS)
		target_link_libraries(${target} PRIVATE ${ARG_LINKS})
	endif()

	if(ARG_DEFINES)
		target_compile_definitions(${target} PRIVATE ${ARG_DEFINES})
	endif()
endfunction()

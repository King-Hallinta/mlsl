# mlsl
# SPDX-FileCopyrightText: 2026 Korrikada
# SPDX-License-Identifier: Apache-2.0

file(GLOB_RECURSE MLSL_TEST_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

mlsl_console_application(mlsl_tests 23
	SOURCES ${MLSL_TEST_SOURCES}
	LINKS
		mlsl
		Catch2::Catch2)

add_test(NAME mlsl_tests COMMAND mlsl_tests)

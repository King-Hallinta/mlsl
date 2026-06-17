// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdlib>

#if defined(_MSC_VER)
	#include <malloc.h>
	#define MLSL_RESTRICT __restrict
	#define MLSL_ALIGNED_ALLOC(size, alignment) _aligned_malloc(size, alignment)
	#define MLSL_ALIGNED_FREE(ptr) _aligned_free(ptr)
#elif defined(__GNUC__) || defined(__clang__)
	#define MLSL_RESTRICT __restrict__
	#define MLSL_ALIGNED_ALLOC(size, alignment) std::aligned_alloc(alignment, size)
	#define MLSL_ALIGNED_FREE(ptr) std::free(ptr)
#else
	#define MLSL_RESTRICT
	#define MLSL_ALIGNED_ALLOC(size, alignment) std::aligned_alloc(alignment, size)
	#define MLSL_ALIGNED_FREE(ptr) std::free(ptr)
#endif

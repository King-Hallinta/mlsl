// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	enum class ErrorType
	{
		None,
		OutOfMemory,
		InvalidArgument,
		OutOfBounds,
		NullPointer,
		InvalidOperation,
		NotFound,
		AlreadyExists,
		BufferOverflow,
		AllocationFailed
	};

	struct Error
	{
		ErrorType type;
	};
} // namespace mlsl

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>

namespace mlsl
{
	[[nodiscard]] constexpr std::uintptr_t AlignAddress(std::uintptr_t address, std::size_t alignment)
	{
		auto remainder = address % alignment;

		if (remainder == 0)
		{
			return address;
		}

		return address + alignment - remainder;
	}

	[[nodiscard]] constexpr std::size_t AlignSize(std::size_t size, std::size_t alignment)
	{
		auto remainder = size % alignment;

		if (remainder == 0)
		{
			return size;
		}

		return size + alignment - remainder;
	}

	[[nodiscard]] constexpr bool IsAligned(std::uintptr_t address, std::size_t alignment)
	{
		return address % alignment == 0;
	}
} // namespace mlsl

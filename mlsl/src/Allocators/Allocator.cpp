// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Allocators/Allocator.hpp>

#include <cstring>

namespace mlsl
{
	std::expected<void *MLSL_RESTRICT, Error> Allocator::Reallocate(void *ptr, std::size_t oldSize, std::size_t newSize, std::size_t alignment)
	{
		if (newSize == 0)
		{
			Deallocate(ptr, oldSize);
			return nullptr;
		}

		if (ptr == nullptr)
		{
			return Allocate(newSize, alignment);
		}

		auto result = Allocate(newSize, alignment);

		if (!result)
		{
			return result;
		}

		std::memcpy(*result, ptr, oldSize < newSize ? oldSize : newSize);
		Deallocate(ptr, oldSize);

		return result;
	}
} // namespace mlsl

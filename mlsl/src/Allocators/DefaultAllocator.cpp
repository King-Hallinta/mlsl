// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Allocators/DefaultAllocator.hpp>

namespace mlsl
{
	std::expected<void *MLSL_RESTRICT, Error> DefaultAllocator::Allocate(std::size_t size, std::size_t alignment)
	{
		if (alignment == 0)
		{
			return std::unexpected(Error {ErrorType::InvalidArgument});
		}

		if (size == 0)
		{
			return nullptr;
		}

		void *ptr = MLSL_ALIGNED_ALLOC(size, alignment);

		if (ptr == nullptr)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		return ptr;
	}

	void DefaultAllocator::Deallocate(void *ptr, std::size_t size)
	{
		(void)size;

		if (ptr == nullptr)
		{
			return;
		}

		MLSL_ALIGNED_FREE(ptr);
	}

	bool DefaultAllocator::Owns(void *ptr) const
	{
		(void)ptr;
		return true;
	}
} // namespace mlsl

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>

namespace mlsl
{
	class DefaultAllocator : public Allocator
	{
	public:
		[[nodiscard]] std::expected<void *MLSL_RESTRICT, Error> Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override;
		void Deallocate(void *ptr, std::size_t size) override;
		[[nodiscard]] bool Owns(void *ptr) const override;
	};
} // namespace mlsl

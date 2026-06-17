// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Core/Error.hpp>
#include <mlsl/Core/Macros.hpp>

namespace mlsl
{
	class Allocator
	{
	public:
		virtual ~Allocator() = default;

	public:
		[[nodiscard]] virtual std::expected<void *MLSL_RESTRICT, Error> Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) = 0;
		virtual void Deallocate(void *ptr, std::size_t size) = 0;
		[[nodiscard]] virtual std::expected<void *MLSL_RESTRICT, Error> Reallocate(void *ptr, std::size_t oldSize, std::size_t newSize, std::size_t alignment = alignof(std::max_align_t));
		[[nodiscard]] virtual bool Owns(void *ptr) const = 0;
	};
} // namespace mlsl

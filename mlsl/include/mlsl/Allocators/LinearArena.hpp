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
	class LinearArena
	{
	public:
		LinearArena() = default;
		LinearArena(void *buffer, std::size_t size);
		LinearArena(const LinearArena &) = delete;
		LinearArena(LinearArena &&other) noexcept;
		~LinearArena() = default;

	public:
		LinearArena &operator=(const LinearArena &) = delete;
		LinearArena &operator=(LinearArena &&other) noexcept;

	public:
		[[nodiscard]] std::expected<void *MLSL_RESTRICT, Error> Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t));
		[[nodiscard]] bool Owns(void *ptr) const;

		void Reset();
		[[nodiscard]] std::size_t Used() const;
		[[nodiscard]] std::size_t Remaining() const;
		[[nodiscard]] std::size_t Capacity() const;

	private:
		void *m_Buffer = nullptr;
		std::size_t m_Size = 0;
		std::size_t m_Offset = 0;
	};
} // namespace mlsl

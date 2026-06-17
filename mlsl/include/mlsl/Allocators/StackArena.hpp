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
	class StackArena
	{
	public:
		struct Marker
		{
			std::size_t offset;
		};

	public:
		StackArena() = default;
		StackArena(void *buffer, std::size_t size);
		StackArena(const StackArena &) = delete;
		StackArena(StackArena &&other) noexcept;
		~StackArena() = default;

	public:
		StackArena &operator=(const StackArena &) = delete;
		StackArena &operator=(StackArena &&other) noexcept;

	public:
		[[nodiscard]] std::expected<void *MLSL_RESTRICT, Error> Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t));
		[[nodiscard]] bool Owns(void *ptr) const;

		[[nodiscard]] Marker GetMarker() const;
		void FreeToMarker(Marker marker);
		void Reset();

		[[nodiscard]] std::size_t Used() const;
		[[nodiscard]] std::size_t Capacity() const;

	private:
		void *m_Buffer = nullptr;
		std::size_t m_Size = 0;
		std::size_t m_Offset = 0;
	};
} // namespace mlsl

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>

namespace mlsl
{
	template <std::size_t Capacity, std::size_t BufferAlignment = alignof(std::max_align_t)>
	class StaticBuffer
	{
	public:
		using SizeType = std::size_t;
		using Pointer = std::byte *;
		using ConstPointer = const std::byte *;

	public:
		static_assert(Capacity > 0);
		static_assert(BufferAlignment > 0);

	public:
		[[nodiscard]] constexpr Pointer Data();
		[[nodiscard]] constexpr ConstPointer Data() const;
		[[nodiscard]] constexpr SizeType Size() const;
		[[nodiscard]] constexpr SizeType Alignment() const;

	private:
		alignas(BufferAlignment) std::byte m_Data[Capacity] {};
	};

} // namespace mlsl

#include <mlsl/Memory/StaticBuffer.inl>

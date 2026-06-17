// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename T>
	class Span
	{
	public:
		using ValueType = T;
		using SizeType = std::size_t;
		using Reference = T &;
		using ConstReference = const T &;
		using Pointer = T *;
		using ConstPointer = const T *;
		using Iterator = T *;
		using ConstIterator = const T *;

	public:
		constexpr Span() = default;
		constexpr Span(Pointer data, SizeType size);
		template <std::size_t N>
		constexpr Span(T (&arr)[N]);

	public:
		[[nodiscard]] constexpr std::expected<Pointer, Error> Get(SizeType index);
		[[nodiscard]] constexpr std::expected<ConstPointer, Error> Get(SizeType index) const;
		[[nodiscard]] constexpr Reference operator[](SizeType index);
		[[nodiscard]] constexpr ConstReference operator[](SizeType index) const;
		[[nodiscard]] constexpr Reference Front();
		[[nodiscard]] constexpr ConstReference Front() const;
		[[nodiscard]] constexpr Reference Back();
		[[nodiscard]] constexpr ConstReference Back() const;
		[[nodiscard]] constexpr Pointer Data();
		[[nodiscard]] constexpr ConstPointer Data() const;

		[[nodiscard]] constexpr Iterator Begin();
		[[nodiscard]] constexpr ConstIterator Begin() const;
		[[nodiscard]] constexpr Iterator End();
		[[nodiscard]] constexpr ConstIterator End() const;

		[[nodiscard]] constexpr SizeType Size() const;
		[[nodiscard]] constexpr SizeType SizeBytes() const;
		[[nodiscard]] constexpr bool Empty() const;

		[[nodiscard]] constexpr Span Subspan(SizeType offset, SizeType count) const;
		[[nodiscard]] constexpr Span First(SizeType count) const;
		[[nodiscard]] constexpr Span Last(SizeType count) const;

	private:
		Pointer m_Data = nullptr;
		SizeType m_Size = 0;
	};
} // namespace mlsl

#include <mlsl/Memory/Span.inl>

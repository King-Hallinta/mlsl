// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>
#include <initializer_list>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class Array
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
		constexpr Array() = default;
		constexpr Array(std::initializer_list<T> init);

	public:
		[[nodiscard]] constexpr Reference operator[](SizeType index);
		[[nodiscard]] constexpr ConstReference operator[](SizeType index) const;

	public:
		[[nodiscard]] constexpr std::expected<Pointer, Error> Get(SizeType index);
		[[nodiscard]] constexpr std::expected<ConstPointer, Error> Get(SizeType index) const;
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
		[[nodiscard]] constexpr bool Empty() const;

		constexpr void Fill(const T &value);

	private:
		T m_Data[N];
	};
} // namespace mlsl

#include <mlsl/Containers/Array.inl>

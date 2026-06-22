// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename CharT>
	class BasicStringView
	{
	public:
		using ValueType = CharT;
		using SizeType = std::size_t;
		using ConstReference = const CharT &;
		using ConstPointer = const CharT *;
		using ConstIterator = const CharT *;

	public:
		static constexpr SizeType Npos = static_cast<SizeType>(-1);

	public:
		constexpr BasicStringView() = default;
		constexpr BasicStringView(std::nullptr_t);
		constexpr BasicStringView(ConstPointer data);
		constexpr BasicStringView(ConstPointer data, SizeType size);
		constexpr BasicStringView(ConstPointer begin, ConstPointer end);

	public:
		[[nodiscard]] constexpr ConstReference operator[](SizeType index) const;

	public:
		[[nodiscard]] constexpr std::expected<ConstPointer, Error> Get(SizeType index) const;
		[[nodiscard]] constexpr ConstReference Front() const;
		[[nodiscard]] constexpr ConstReference Back() const;
		[[nodiscard]] constexpr ConstPointer Data() const;

		[[nodiscard]] constexpr ConstIterator Begin() const;
		[[nodiscard]] constexpr ConstIterator End() const;

		[[nodiscard]] constexpr SizeType Size() const;
		[[nodiscard]] constexpr bool Empty() const;

		[[nodiscard]] constexpr std::expected<BasicStringView, Error> Slice(SizeType offset, SizeType count = Npos) const;
		[[nodiscard]] constexpr std::expected<BasicStringView, Error> Substr(SizeType offset, SizeType count = Npos) const;
		[[nodiscard]] constexpr bool Equals(BasicStringView other) const;
		[[nodiscard]] constexpr int Compare(BasicStringView other) const;

		[[nodiscard]] constexpr bool operator==(BasicStringView other) const;
		[[nodiscard]] constexpr bool operator!=(BasicStringView other) const;

	private:
		[[nodiscard]] static constexpr SizeType Length(ConstPointer data);

	private:
		ConstPointer m_Data = nullptr;
		SizeType m_Size = 0;
	};
} // namespace mlsl

#include <mlsl/Strings/BasicStringView.inl>

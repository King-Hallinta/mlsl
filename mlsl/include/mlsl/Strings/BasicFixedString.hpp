// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Core/Error.hpp>
#include <mlsl/Strings/BasicStringView.hpp>

namespace mlsl
{
	template <typename CharT, std::size_t N>
	class BasicFixedString
	{
	public:
		using ValueType = CharT;
		using SizeType = std::size_t;
		using Reference = CharT &;
		using ConstReference = const CharT &;
		using Pointer = CharT *;
		using ConstPointer = const CharT *;
		using Iterator = CharT *;
		using ConstIterator = const CharT *;
		using ViewType = BasicStringView<CharT>;

	public:
		static constexpr SizeType Npos = static_cast<SizeType>(-1);

	public:
		constexpr BasicFixedString();
		constexpr BasicFixedString(std::nullptr_t);
		constexpr BasicFixedString(ConstPointer data);
		constexpr BasicFixedString(ViewType view);

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
		[[nodiscard]] constexpr ConstPointer CString() const;

		[[nodiscard]] constexpr Iterator Begin();
		[[nodiscard]] constexpr ConstIterator Begin() const;
		[[nodiscard]] constexpr Iterator End();
		[[nodiscard]] constexpr ConstIterator End() const;

		[[nodiscard]] constexpr SizeType Size() const;
		[[nodiscard]] constexpr SizeType Capacity() const;
		[[nodiscard]] constexpr bool Empty() const;

		constexpr std::expected<void, Error> Reserve(SizeType capacity);
		constexpr void Clear();
		constexpr std::expected<void, Error> Assign(ConstPointer data);
		constexpr std::expected<void, Error> Assign(ViewType view);
		constexpr std::expected<void, Error> Append(ConstPointer data);
		constexpr std::expected<void, Error> Append(ViewType view);
		constexpr std::expected<void, Error> Append(CharT value);
		constexpr void Remove();
		constexpr std::expected<void, Error> Resize(SizeType size, CharT value = CharT {});

		[[nodiscard]] constexpr ViewType View() const;
		[[nodiscard]] constexpr std::expected<ViewType, Error> Substr(SizeType offset, SizeType count = Npos) const;
		[[nodiscard]] constexpr bool Equals(ViewType other) const;
		[[nodiscard]] constexpr int Compare(ViewType other) const;

		[[nodiscard]] constexpr bool operator==(ViewType other) const;
		[[nodiscard]] constexpr bool operator!=(ViewType other) const;

	private:
		constexpr void WriteTerminator();

	private:
		CharT m_Data[N + 1] {};
		SizeType m_Size = 0;
	};
} // namespace mlsl

#include <mlsl/Strings/BasicFixedString.inl>

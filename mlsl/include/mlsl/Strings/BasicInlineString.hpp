// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <expected>
#include <utility>

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Core/Error.hpp>
#include <mlsl/Strings/BasicStringView.hpp>

namespace mlsl
{
	template <typename CharT, std::size_t N>
	class BasicInlineString
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
		BasicInlineString();
		explicit BasicInlineString(Allocator *allocator);
		BasicInlineString(std::nullptr_t);
		BasicInlineString(ConstPointer data);
		BasicInlineString(ViewType view);
		BasicInlineString(const BasicInlineString &other);
		BasicInlineString(BasicInlineString &&other) noexcept;
		~BasicInlineString();

	public:
		BasicInlineString &operator=(const BasicInlineString &other);
		BasicInlineString &operator=(BasicInlineString &&other) noexcept;

	public:
		[[nodiscard]] Reference operator[](SizeType index);
		[[nodiscard]] ConstReference operator[](SizeType index) const;

	public:
		[[nodiscard]] std::expected<Pointer, Error> Get(SizeType index);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(SizeType index) const;
		[[nodiscard]] Reference Front();
		[[nodiscard]] ConstReference Front() const;
		[[nodiscard]] Reference Back();
		[[nodiscard]] ConstReference Back() const;
		[[nodiscard]] Pointer Data();
		[[nodiscard]] ConstPointer Data() const;
		[[nodiscard]] ConstPointer CString() const;

		[[nodiscard]] Iterator Begin();
		[[nodiscard]] ConstIterator Begin() const;
		[[nodiscard]] Iterator End();
		[[nodiscard]] ConstIterator End() const;

		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] SizeType InlineCapacity() const;
		[[nodiscard]] bool Empty() const;

		std::expected<void, Error> Reserve(SizeType capacity);
		void Clear();
		std::expected<void, Error> Assign(ConstPointer data);
		std::expected<void, Error> Assign(ViewType view);
		std::expected<void, Error> Append(ConstPointer data);
		std::expected<void, Error> Append(ViewType view);
		std::expected<void, Error> Add(CharT value);
		void Remove();
		std::expected<void, Error> Resize(SizeType size, CharT value = CharT {});

		[[nodiscard]] ViewType View() const;
		[[nodiscard]] std::expected<ViewType, Error> Substr(SizeType offset, SizeType count = Npos) const;
		[[nodiscard]] bool Equals(ViewType other) const;
		[[nodiscard]] int Compare(ViewType other) const;

		[[nodiscard]] bool operator==(ViewType other) const;
		[[nodiscard]] bool operator!=(ViewType other) const;

	private:
		[[nodiscard]] bool IsInline() const;
		std::expected<void, Error> Grow(SizeType capacity);
		void ReleaseHeapStorage();
		void WriteTerminator();

	private:
		CharT m_InlineStorage[N + 1] {};
		Pointer m_Data = m_InlineStorage;
		SizeType m_Size = 0;
		SizeType m_Capacity = N;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;
	};
} // namespace mlsl

#include <mlsl/Strings/BasicInlineString.inl>

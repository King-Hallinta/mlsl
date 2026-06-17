// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>
#include <functional>
#include <utility>

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename LessT = std::less<KeyT>>
	class InlineMap
	{
	public:
		using KeyType = KeyT;
		using ValueType = ValueT;
		using SizeType = std::size_t;
		using LessType = LessT;
		using Reference = ValueT &;
		using ConstReference = const ValueT &;
		using Pointer = ValueT *;
		using ConstPointer = const ValueT *;

	private:
		struct Entry
		{
			KeyT key;
			ValueT value;
		};

		static constexpr SizeType InlineStorageCount = N == 0 ? 1 : N;

	public:
		InlineMap();
		explicit InlineMap(Allocator *allocator);
		InlineMap(LessT less);
		InlineMap(Allocator *allocator, LessT less);
		InlineMap(const InlineMap &other);
		InlineMap(InlineMap &&other) noexcept;
		~InlineMap();

	public:
		InlineMap &operator=(const InlineMap &other);
		InlineMap &operator=(InlineMap &&other) noexcept;

	public:
		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] SizeType InlineCapacity() const;
		[[nodiscard]] bool Empty() const;

		void Clear();
		[[nodiscard]] std::expected<void, Error> Reserve(SizeType capacity);

		[[nodiscard]] bool Contains(const KeyT &key) const;
		[[nodiscard]] std::expected<Pointer, Error> Get(const KeyT &key);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(const KeyT &key) const;

		std::expected<void, Error> Add(const KeyT &key, const ValueT &value);
		std::expected<void, Error> Add(const KeyT &key, ValueT &&value);
		std::expected<void, Error> Add(KeyT &&key, const ValueT &value);
		std::expected<void, Error> Add(KeyT &&key, ValueT &&value);
		[[nodiscard]] bool Remove(const KeyT &key);

	private:
		[[nodiscard]] Entry *InlineEntries();
		[[nodiscard]] const Entry *InlineEntries() const;
		[[nodiscard]] bool IsInline() const;
		[[nodiscard]] bool KeysEqual(const KeyT &left, const KeyT &right) const;
		[[nodiscard]] SizeType FindIndex(const KeyT &key) const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);
		[[nodiscard]] std::expected<Entry *, Error> AllocateEntries(SizeType capacity);

		template <typename KeyArg, typename ValueArg>
		std::expected<void, Error> AddInternal(KeyArg &&key, ValueArg &&value);

		void DestroyEntries();
		void ReleaseHeapEntries();
		std::expected<void, Error> Grow(SizeType capacity);
		void CopyFrom(const InlineMap &other);
		void MoveFrom(InlineMap &other);

	private:
		alignas(Entry) std::byte m_InlineStorage[sizeof(Entry) * InlineStorageCount];
		Entry *m_Entries = InlineEntries();
		SizeType m_Size = 0;
		SizeType m_Capacity = N;
		LessT m_Less {};
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;
	};
} // namespace mlsl

#include <mlsl/Containers/InlineMap.inl>

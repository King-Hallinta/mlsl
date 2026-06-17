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
	template <typename KeyT, std::size_t N, typename LessT = std::less<KeyT>>
	class InlineSet
	{
	public:
		using KeyType = KeyT;
		using ValueType = KeyT;
		using SizeType = std::size_t;
		using LessType = LessT;
		using Pointer = KeyT *;
		using ConstPointer = const KeyT *;

	private:
		static constexpr SizeType InlineStorageCount = N == 0 ? 1 : N;

	public:
		InlineSet();
		explicit InlineSet(Allocator *allocator);
		InlineSet(LessT less);
		InlineSet(Allocator *allocator, LessT less);
		InlineSet(const InlineSet &other);
		InlineSet(InlineSet &&other) noexcept;
		~InlineSet();

	public:
		InlineSet &operator=(const InlineSet &other);
		InlineSet &operator=(InlineSet &&other) noexcept;

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

		std::expected<void, Error> Add(const KeyT &key);
		std::expected<void, Error> Add(KeyT &&key);
		[[nodiscard]] bool Remove(const KeyT &key);

	private:
		[[nodiscard]] KeyT *InlineKeys();
		[[nodiscard]] const KeyT *InlineKeys() const;
		[[nodiscard]] bool IsInline() const;
		[[nodiscard]] bool KeysEqual(const KeyT &left, const KeyT &right) const;
		[[nodiscard]] SizeType FindIndex(const KeyT &key) const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);
		[[nodiscard]] std::expected<KeyT *, Error> AllocateKeys(SizeType capacity);

		template <typename KeyArg>
		std::expected<void, Error> AddInternal(KeyArg &&key);

		void DestroyKeys();
		void ReleaseHeapKeys();
		std::expected<void, Error> Grow(SizeType capacity);
		void CopyFrom(const InlineSet &other);
		void MoveFrom(InlineSet &other);

	private:
		alignas(KeyT) std::byte m_InlineStorage[sizeof(KeyT) * InlineStorageCount];
		KeyT *m_Keys = InlineKeys();
		SizeType m_Size = 0;
		SizeType m_Capacity = N;
		LessT m_Less {};
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;
	};
} // namespace mlsl

#include <mlsl/Containers/InlineSet.inl>

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>
#include <functional>
#include <utility>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename LessT = std::less<KeyT>>
	class FixedMap
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

		static constexpr SizeType StorageCount = N == 0 ? 1 : N;

	public:
		FixedMap() = default;
		FixedMap(LessT less);
		FixedMap(const FixedMap &other);
		FixedMap(FixedMap &&other) noexcept;
		~FixedMap();

	public:
		FixedMap &operator=(const FixedMap &other);
		FixedMap &operator=(FixedMap &&other) noexcept;

	public:
		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] bool Empty() const;
		[[nodiscard]] bool Full() const;

		void Clear();

		[[nodiscard]] bool Contains(const KeyT &key) const;
		[[nodiscard]] std::expected<Pointer, Error> Get(const KeyT &key);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(const KeyT &key) const;

		std::expected<void, Error> Add(const KeyT &key, const ValueT &value);
		std::expected<void, Error> Add(const KeyT &key, ValueT &&value);
		std::expected<void, Error> Add(KeyT &&key, const ValueT &value);
		std::expected<void, Error> Add(KeyT &&key, ValueT &&value);
		[[nodiscard]] bool Remove(const KeyT &key);

	private:
		[[nodiscard]] Entry *Entries();
		[[nodiscard]] const Entry *Entries() const;
		[[nodiscard]] bool KeysEqual(const KeyT &left, const KeyT &right) const;
		[[nodiscard]] SizeType FindIndex(const KeyT &key) const;

		template <typename KeyArg, typename ValueArg>
		std::expected<void, Error> AddInternal(KeyArg &&key, ValueArg &&value);

		void DestroyEntries();
		void CopyFrom(const FixedMap &other);
		void MoveFrom(FixedMap &other);

	private:
		alignas(Entry) std::byte m_Storage[sizeof(Entry) * StorageCount];
		SizeType m_Size = 0;
		LessT m_Less {};
	};
} // namespace mlsl

#include <mlsl/Containers/FixedMap.inl>

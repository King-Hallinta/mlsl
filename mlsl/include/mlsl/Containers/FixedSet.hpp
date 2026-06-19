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
	template <typename KeyT, std::size_t N, typename LessT = std::less<KeyT>>
	class FixedSet
	{
	public:
		using KeyType = KeyT;
		using ValueType = KeyT;
		using SizeType = std::size_t;
		using LessType = LessT;
		using Pointer = KeyT *;
		using ConstPointer = const KeyT *;

	private:
		static constexpr SizeType StorageCount = N == 0 ? 1 : N;

	public:
		FixedSet() = default;
		FixedSet(LessT less);
		FixedSet(const FixedSet &other);
		FixedSet(FixedSet &&other) noexcept;
		~FixedSet();

	public:
		FixedSet &operator=(const FixedSet &other);
		FixedSet &operator=(FixedSet &&other) noexcept;

	public:
		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] bool Empty() const;
		[[nodiscard]] bool Full() const;

		void Clear();

		[[nodiscard]] bool Contains(const KeyT &key) const;
		[[nodiscard]] std::expected<Pointer, Error> Get(const KeyT &key);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(const KeyT &key) const;

		std::expected<void, Error> Add(const KeyT &key);
		std::expected<void, Error> Add(KeyT &&key);
		[[nodiscard]] bool Remove(const KeyT &key);

	private:
		[[nodiscard]] KeyT *Keys();
		[[nodiscard]] const KeyT *Keys() const;
		[[nodiscard]] bool KeysEqual(const KeyT &left, const KeyT &right) const;
		[[nodiscard]] SizeType FindIndex(const KeyT &key) const;

		template <typename KeyArg>
		std::expected<void, Error> AddInternal(KeyArg &&key);

		void DestroyKeys();
		void CopyFrom(const FixedSet &other);
		void MoveFrom(FixedSet &other);

	private:
		alignas(KeyT) std::byte m_Storage[sizeof(KeyT) * StorageCount];
		SizeType m_Size = 0;
		LessT m_Less {};
	};
} // namespace mlsl

#include <mlsl/Containers/FixedSet.inl>

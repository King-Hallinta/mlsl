// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>
#include <functional>
#include <new>
#include <utility>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, typename Derived, typename LessT = std::less<KeyT>>
	class BasicMap
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

	public:
		BasicMap() = default;
		BasicMap(LessT less);

	public:
		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
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

	protected:
		struct Entry
		{
			KeyT key;
			ValueT value;
		};

	protected:
		void DestroyEntries();

	protected:
		SizeType m_Size = 0;
		LessT m_Less {};

	private:
		[[nodiscard]] Derived &AsDerived();
		[[nodiscard]] const Derived &AsDerived() const;

		[[nodiscard]] bool KeysEqual(const KeyT &left, const KeyT &right) const;
		[[nodiscard]] SizeType FindIndex(const KeyT &key) const;

		template <typename KeyArg, typename ValueArg>
		std::expected<void, Error> AddInternal(KeyArg &&key, ValueArg &&value);

	private:
		friend Derived;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicMap.inl>

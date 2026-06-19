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
	template <typename KeyT, typename Derived, typename LessT = std::less<KeyT>>
	class BasicSet
	{
	public:
		using KeyType = KeyT;
		using ValueType = KeyT;
		using SizeType = std::size_t;
		using LessType = LessT;
		using Pointer = KeyT *;
		using ConstPointer = const KeyT *;

	public:
		BasicSet() = default;
		BasicSet(LessT less);

	public:
		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] bool Empty() const;

		void Clear();
		[[nodiscard]] std::expected<void, Error> Reserve(SizeType capacity);

		[[nodiscard]] bool Contains(const KeyT &key) const;
		[[nodiscard]] std::expected<Pointer, Error> Get(const KeyT &key);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(const KeyT &key) const;

		std::expected<void, Error> Add(const KeyT &key);
		std::expected<void, Error> Add(KeyT &&key);
		[[nodiscard]] bool Remove(const KeyT &key);

	protected:
		void DestroyKeys();

	protected:
		SizeType m_Size = 0;
		LessT m_Less {};

	private:
		[[nodiscard]] Derived &AsDerived();
		[[nodiscard]] const Derived &AsDerived() const;

		[[nodiscard]] bool KeysEqual(const KeyT &left, const KeyT &right) const;
		[[nodiscard]] SizeType FindIndex(const KeyT &key) const;

		template <typename KeyArg>
		std::expected<void, Error> AddInternal(KeyArg &&key);

	private:
		friend Derived;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicSet.inl>

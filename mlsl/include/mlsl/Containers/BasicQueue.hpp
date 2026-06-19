// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Containers/BasicDeque.hpp>

namespace mlsl
{
	template <typename T, typename Derived>
	class BasicQueue : protected BasicDeque<T, Derived>
	{
	public:
		using Base = BasicDeque<T, Derived>;
		using ValueType = T;
		using SizeType = std::size_t;
		using Reference = T &;
		using ConstReference = const T &;
		using Iterator = typename Base::Iterator;
		using ConstIterator = typename Base::ConstIterator;

	public:
		[[nodiscard]] Reference Front();
		[[nodiscard]] ConstReference Front() const;
		[[nodiscard]] Reference Back();
		[[nodiscard]] ConstReference Back() const;

		[[nodiscard]] Iterator Begin();
		[[nodiscard]] ConstIterator Begin() const;
		[[nodiscard]] Iterator End();
		[[nodiscard]] ConstIterator End() const;

		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] bool Empty() const;

		std::expected<void, Error> Add(const T &value);
		std::expected<void, Error> Add(T &&value);
		void Remove();
		void Clear();

	protected:
		BasicQueue() = default;
		~BasicQueue() = default;

	protected:
		[[nodiscard]] SizeType MapIndex(SizeType index) const;

	private:
		friend class BasicDeque<T, Derived>;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicQueue.inl>

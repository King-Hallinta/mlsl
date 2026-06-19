// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename T, typename Derived>
	class BasicStack
	{
	public:
		using ValueType = T;
		using SizeType = std::size_t;
		using Reference = T &;
		using ConstReference = const T &;
		using Pointer = T *;
		using ConstPointer = const T *;
		using Iterator = T *;
		using ConstIterator = const T *;

	public:
		[[nodiscard]] Reference Top();
		[[nodiscard]] ConstReference Top() const;

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
		BasicStack() = default;
		~BasicStack() = default;

	protected:
		Derived &AsDerived();
		const Derived &AsDerived() const;

	protected:
		SizeType m_Size = 0;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicStack.inl>

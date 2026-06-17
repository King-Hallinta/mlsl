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
	class BasicDeque
	{
	public:
		using ValueType = T;
		using SizeType = std::size_t;
		using Reference = T &;
		using ConstReference = const T &;
		using Pointer = T *;
		using ConstPointer = const T *;

	public:
		[[nodiscard]] std::expected<Pointer, Error> Get(SizeType index);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(SizeType index) const;
		[[nodiscard]] Reference operator[](SizeType index);
		[[nodiscard]] ConstReference operator[](SizeType index) const;
		[[nodiscard]] Reference Front();
		[[nodiscard]] ConstReference Front() const;
		[[nodiscard]] Reference Back();
		[[nodiscard]] ConstReference Back() const;

		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] bool Empty() const;

		std::expected<void, Error> AddFront(const T &value);
		std::expected<void, Error> AddFront(T &&value);
		std::expected<void, Error> AddBack(const T &value);
		std::expected<void, Error> AddBack(T &&value);
		void RemoveFront();
		void RemoveBack();
		void Clear();

	protected:
		BasicDeque() = default;
		~BasicDeque() = default;

	protected:
		Derived &AsDerived();
		const Derived &AsDerived() const;

		[[nodiscard]] SizeType MapIndex(SizeType index) const;

	protected:
		SizeType m_Head = 0;
		SizeType m_Size = 0;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicDeque.inl>

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

		class Iterator
		{
		public:
			Iterator(BasicDeque *deque, SizeType index);

		public:
			Reference operator*();
			Pointer operator->();
			Iterator &operator++();
			Iterator operator++(int);
			Iterator &operator--();
			Iterator operator--(int);
			bool operator==(const Iterator &other) const;
			bool operator!=(const Iterator &other) const;

		private:
			BasicDeque *m_Deque;
			SizeType m_Index;

		private:
			friend class BasicDeque;
		};

		class ConstIterator
		{
		public:
			ConstIterator(const BasicDeque *deque, SizeType index);

		public:
			ConstReference operator*() const;
			ConstPointer operator->() const;
			ConstIterator &operator++();
			ConstIterator operator++(int);
			ConstIterator &operator--();
			ConstIterator operator--(int);
			bool operator==(const ConstIterator &other) const;
			bool operator!=(const ConstIterator &other) const;

		private:
			const BasicDeque *m_Deque;
			SizeType m_Index;

		private:
			friend class BasicDeque;
		};

	public:
		[[nodiscard]] std::expected<Pointer, Error> Get(SizeType index);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(SizeType index) const;
		[[nodiscard]] Reference operator[](SizeType index);
		[[nodiscard]] ConstReference operator[](SizeType index) const;
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

		std::expected<void, Error> Insert(const T &value);
		std::expected<void, Error> Insert(T &&value);
		std::expected<void, Error> Append(const T &value);
		std::expected<void, Error> Append(T &&value);
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

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename T>
	struct ListNode
	{
		T value;
		ListNode *prev;
		ListNode *next;
	};

	template <typename T, typename Derived>
	class BasicList
	{
	public:
		using ValueType = T;
		using SizeType = std::size_t;
		using Reference = T &;
		using ConstReference = const T &;
		using Pointer = T *;
		using ConstPointer = const T *;
		using NodeType = ListNode<T>;

		class Iterator
		{
		public:
			Iterator(NodeType *node);

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
			NodeType *m_Node;

		private:
			friend class BasicList;
		};

		class ConstIterator
		{
		public:
			ConstIterator(const NodeType *node);

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
			const NodeType *m_Node;

		private:
			friend class BasicList;
		};

	public:
		[[nodiscard]] std::expected<Pointer, Error> Get(SizeType index);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(SizeType index) const;
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

		std::expected<Iterator, Error> Insert(Iterator pos, const T &value);
		std::expected<Iterator, Error> Insert(Iterator pos, T &&value);
		Iterator Erase(Iterator pos);
		void Clear();

	protected:
		BasicList() = default;
		~BasicList() = default;

	protected:
		Derived &AsDerived();
		const Derived &AsDerived() const;

		void LinkNode(NodeType *pos, NodeType *node);
		void UnlinkNode(NodeType *node);

	protected:
		NodeType *m_Head = nullptr;
		NodeType *m_Tail = nullptr;
		SizeType m_Size = 0;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicList.inl>

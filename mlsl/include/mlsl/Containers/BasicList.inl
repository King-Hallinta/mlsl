// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>

namespace mlsl
{
	template <typename T, typename Derived>
	BasicList<T, Derived>::Iterator::Iterator(NodeType *node)
		: m_Node(node)
	{
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Reference BasicList<T, Derived>::Iterator::operator*()
	{
		return m_Node->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Pointer BasicList<T, Derived>::Iterator::operator->()
	{
		return &m_Node->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Iterator &BasicList<T, Derived>::Iterator::operator++()
	{
		m_Node = m_Node->next;

		return *this;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Iterator BasicList<T, Derived>::Iterator::operator++(int)
	{
		Iterator tmp = *this;
		m_Node = m_Node->next;

		return tmp;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Iterator &BasicList<T, Derived>::Iterator::operator--()
	{
		m_Node = m_Node->prev;

		return *this;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Iterator BasicList<T, Derived>::Iterator::operator--(int)
	{
		Iterator tmp = *this;
		m_Node = m_Node->prev;

		return tmp;
	}

	template <typename T, typename Derived>
	bool BasicList<T, Derived>::Iterator::operator==(const Iterator &other) const
	{
		return m_Node == other.m_Node;
	}

	template <typename T, typename Derived>
	bool BasicList<T, Derived>::Iterator::operator!=(const Iterator &other) const
	{
		return m_Node != other.m_Node;
	}

	template <typename T, typename Derived>
	BasicList<T, Derived>::ConstIterator::ConstIterator(const NodeType *node)
		: m_Node(node)
	{
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstReference BasicList<T, Derived>::ConstIterator::operator*() const
	{
		return m_Node->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstPointer BasicList<T, Derived>::ConstIterator::operator->() const
	{
		return &m_Node->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstIterator &BasicList<T, Derived>::ConstIterator::operator++()
	{
		m_Node = m_Node->next;

		return *this;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstIterator BasicList<T, Derived>::ConstIterator::operator++(int)
	{
		ConstIterator tmp = *this;
		m_Node = m_Node->next;

		return tmp;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstIterator &BasicList<T, Derived>::ConstIterator::operator--()
	{
		m_Node = m_Node->prev;

		return *this;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstIterator BasicList<T, Derived>::ConstIterator::operator--(int)
	{
		ConstIterator tmp = *this;
		m_Node = m_Node->prev;

		return tmp;
	}

	template <typename T, typename Derived>
	bool BasicList<T, Derived>::ConstIterator::operator==(const ConstIterator &other) const
	{
		return m_Node == other.m_Node;
	}

	template <typename T, typename Derived>
	bool BasicList<T, Derived>::ConstIterator::operator!=(const ConstIterator &other) const
	{
		return m_Node != other.m_Node;
	}

	template <typename T, typename Derived>
	std::expected<typename BasicList<T, Derived>::Pointer, Error> BasicList<T, Derived>::Get(SizeType index)
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		NodeType *node = m_Head;

		for (SizeType i = 0; i < index; ++i)
		{
			node = node->next;
		}

		return &node->value;
	}

	template <typename T, typename Derived>
	std::expected<typename BasicList<T, Derived>::ConstPointer, Error> BasicList<T, Derived>::Get(SizeType index) const
	{
		if (index >= m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfBounds});
		}

		const NodeType *node = m_Head;

		for (SizeType i = 0; i < index; ++i)
		{
			node = node->next;
		}

		return &node->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Reference BasicList<T, Derived>::Front()
	{
		return m_Head->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstReference BasicList<T, Derived>::Front() const
	{
		return m_Head->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Reference BasicList<T, Derived>::Back()
	{
		return m_Tail->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstReference BasicList<T, Derived>::Back() const
	{
		return m_Tail->value;
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Iterator BasicList<T, Derived>::Begin()
	{
		return Iterator(m_Head);
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstIterator BasicList<T, Derived>::Begin() const
	{
		return ConstIterator(m_Head);
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Iterator BasicList<T, Derived>::End()
	{
		return Iterator(nullptr);
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::ConstIterator BasicList<T, Derived>::End() const
	{
		return ConstIterator(nullptr);
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::SizeType BasicList<T, Derived>::Size() const
	{
		return m_Size;
	}

	template <typename T, typename Derived>
	bool BasicList<T, Derived>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicList<T, Derived>::Insert(const T &value)
	{
		auto result = AsDerived().AllocateNode();

		if (not result)
		{
			return std::unexpected(result.error());
		}

		NodeType *node = *result;
		new (&node->value) T(value);
		node->prev = nullptr;
		node->next = m_Head;

		if (m_Head)
		{
			m_Head->prev = node;
		}
		else
		{
			m_Tail = node;
		}

		m_Head = node;
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicList<T, Derived>::Insert(T &&value)
	{
		auto result = AsDerived().AllocateNode();

		if (not result)
		{
			return std::unexpected(result.error());
		}

		NodeType *node = *result;
		new (&node->value) T(std::move(value));
		node->prev = nullptr;
		node->next = m_Head;

		if (m_Head)
		{
			m_Head->prev = node;
		}
		else
		{
			m_Tail = node;
		}

		m_Head = node;
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicList<T, Derived>::Append(const T &value)
	{
		auto result = AsDerived().AllocateNode();

		if (not result)
		{
			return std::unexpected(result.error());
		}

		NodeType *node = *result;
		new (&node->value) T(value);
		node->prev = m_Tail;
		node->next = nullptr;

		if (m_Tail)
		{
			m_Tail->next = node;
		}
		else
		{
			m_Head = node;
		}

		m_Tail = node;
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	std::expected<void, Error> BasicList<T, Derived>::Append(T &&value)
	{
		auto result = AsDerived().AllocateNode();

		if (not result)
		{
			return std::unexpected(result.error());
		}

		NodeType *node = *result;
		new (&node->value) T(std::move(value));
		node->prev = m_Tail;
		node->next = nullptr;

		if (m_Tail)
		{
			m_Tail->next = node;
		}
		else
		{
			m_Head = node;
		}

		m_Tail = node;
		++m_Size;

		return {};
	}

	template <typename T, typename Derived>
	void BasicList<T, Derived>::RemoveFront()
	{
		if (m_Head == nullptr)
		{
			return;
		}

		NodeType *node = m_Head;
		m_Head = m_Head->next;

		if (m_Head)
		{
			m_Head->prev = nullptr;
		}
		else
		{
			m_Tail = nullptr;
		}

		node->value.~T();
		AsDerived().DeallocateNode(node);
		--m_Size;
	}

	template <typename T, typename Derived>
	void BasicList<T, Derived>::RemoveBack()
	{
		if (m_Tail == nullptr)
		{
			return;
		}

		NodeType *node = m_Tail;
		m_Tail = m_Tail->prev;

		if (m_Tail)
		{
			m_Tail->next = nullptr;
		}
		else
		{
			m_Head = nullptr;
		}

		node->value.~T();
		AsDerived().DeallocateNode(node);
		--m_Size;
	}

	template <typename T, typename Derived>
	std::expected<typename BasicList<T, Derived>::Iterator, Error> BasicList<T, Derived>::Insert(Iterator pos, const T &value)
	{
		if (pos.m_Node == nullptr)
		{
			auto result = Append(value);

			if (not result)
			{
				return std::unexpected(result.error());
			}

			return Iterator(m_Tail);
		}

		if (pos.m_Node == m_Head)
		{
			auto result = Insert(value);

			if (not result)
			{
				return std::unexpected(result.error());
			}

			return Iterator(m_Head);
		}

		auto result = AsDerived().AllocateNode();

		if (not result)
		{
			return std::unexpected(result.error());
		}

		NodeType *node = *result;
		new (&node->value) T(value);

		LinkNode(pos.m_Node, node);
		++m_Size;

		return Iterator(node);
	}

	template <typename T, typename Derived>
	std::expected<typename BasicList<T, Derived>::Iterator, Error> BasicList<T, Derived>::Insert(Iterator pos, T &&value)
	{
		if (pos.m_Node == nullptr)
		{
			auto result = Append(std::move(value));

			if (not result)
			{
				return std::unexpected(result.error());
			}

			return Iterator(m_Tail);
		}

		if (pos.m_Node == m_Head)
		{
			auto result = Insert(std::move(value));

			if (not result)
			{
				return std::unexpected(result.error());
			}

			return Iterator(m_Head);
		}

		auto result = AsDerived().AllocateNode();

		if (not result)
		{
			return std::unexpected(result.error());
		}

		NodeType *node = *result;
		new (&node->value) T(std::move(value));

		LinkNode(pos.m_Node, node);
		++m_Size;

		return Iterator(node);
	}

	template <typename T, typename Derived>
	typename BasicList<T, Derived>::Iterator BasicList<T, Derived>::Erase(Iterator pos)
	{
		if (pos.m_Node == nullptr)
		{
			return End();
		}

		NodeType *next = pos.m_Node->next;

		if (pos.m_Node == m_Head)
		{
			RemoveFront();

			return Iterator(m_Head);
		}

		if (pos.m_Node == m_Tail)
		{
			RemoveBack();

			return End();
		}

		UnlinkNode(pos.m_Node);
		pos.m_Node->value.~T();
		AsDerived().DeallocateNode(pos.m_Node);
		--m_Size;

		return Iterator(next);
	}

	template <typename T, typename Derived>
	void BasicList<T, Derived>::Clear()
	{
		while (m_Head)
		{
			RemoveFront();
		}
	}

	template <typename T, typename Derived>
	Derived &BasicList<T, Derived>::AsDerived()
	{
		return static_cast<Derived &>(*this);
	}

	template <typename T, typename Derived>
	const Derived &BasicList<T, Derived>::AsDerived() const
	{
		return static_cast<const Derived &>(*this);
	}

	template <typename T, typename Derived>
	void BasicList<T, Derived>::LinkNode(NodeType *pos, NodeType *node)
	{
		node->prev = pos->prev;
		node->next = pos;
		pos->prev->next = node;
		pos->prev = node;
	}

	template <typename T, typename Derived>
	void BasicList<T, Derived>::UnlinkNode(NodeType *node)
	{
		if (node->prev)
		{
			node->prev->next = node->next;
		}

		if (node->next)
		{
			node->next->prev = node->prev;
		}
	}
} // namespace mlsl

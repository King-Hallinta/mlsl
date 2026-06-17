// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename T>
	List<T>::List()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename T>
	List<T>::List(Allocator *allocator)
		: m_Allocator(allocator), m_OwnsAllocator(false)
	{
	}

	template <typename T>
	List<T>::List(const List &other)
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		for (auto it = other.Begin(); it != other.End(); ++it)
		{
			static_cast<void>(this->AddBack(*it));
		}
	}

	template <typename T>
	List<T>::List(List &&other) noexcept
		: m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		this->m_Head = other.m_Head;
		this->m_Tail = other.m_Tail;
		this->m_Size = other.m_Size;

		other.m_Head = nullptr;
		other.m_Tail = nullptr;
		other.m_Size = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
	}

	template <typename T>
	List<T>::~List()
	{
		this->Clear();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename T>
	List<T> &List<T>::operator=(const List &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (auto it = other.Begin(); it != other.End(); ++it)
			{
				static_cast<void>(this->AddBack(*it));
			}
		}

		return *this;
	}

	template <typename T>
	List<T> &List<T>::operator=(List &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			if (m_OwnsAllocator)
			{
				delete m_Allocator;
			}

			this->m_Head = other.m_Head;
			this->m_Tail = other.m_Tail;
			this->m_Size = other.m_Size;
			m_Allocator = other.m_Allocator;
			m_OwnsAllocator = other.m_OwnsAllocator;

			other.m_Head = nullptr;
			other.m_Tail = nullptr;
			other.m_Size = 0;
			other.m_Allocator = nullptr;
			other.m_OwnsAllocator = false;
		}

		return *this;
	}

	template <typename T>
	std::expected<typename List<T>::NodeType *, Error> List<T>::AllocateNode()
	{
		auto result = m_Allocator->Allocate(sizeof(NodeType), alignof(NodeType));

		if (!result)
		{
			return std::unexpected(result.error());
		}

		return static_cast<NodeType *>(*result);
	}

	template <typename T>
	void List<T>::DeallocateNode(NodeType *node)
	{
		m_Allocator->Deallocate(node, sizeof(NodeType));
	}
} // namespace mlsl

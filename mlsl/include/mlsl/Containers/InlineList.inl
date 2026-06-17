// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename T, std::size_t N>
	InlineList<T, N>::InlineList()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		for (std::size_t i = 0; i < N - 1; ++i)
		{
			m_Storage[i].next = &m_Storage[i + 1];
		}

		m_Storage[N - 1].next = nullptr;
		m_FreeList = &m_Storage[0];
	}

	template <typename T, std::size_t N>
	InlineList<T, N>::InlineList(Allocator *allocator)
		: m_Allocator(allocator), m_OwnsAllocator(false)
	{
		for (std::size_t i = 0; i < N - 1; ++i)
		{
			m_Storage[i].next = &m_Storage[i + 1];
		}

		m_Storage[N - 1].next = nullptr;
		m_FreeList = &m_Storage[0];
	}

	template <typename T, std::size_t N>
	InlineList<T, N>::InlineList(const InlineList &other)
		: InlineList()
	{
		for (auto it = other.Begin(); it != other.End(); ++it)
		{
			static_cast<void>(this->Append(*it));
		}
	}

	template <typename T, std::size_t N>
	InlineList<T, N>::InlineList(InlineList &&other) noexcept
		: InlineList()
	{
		for (auto it = other.Begin(); it != other.End(); ++it)
		{
			static_cast<void>(this->Append(std::move(*it)));
		}

		other.Clear();
	}

	template <typename T, std::size_t N>
	InlineList<T, N>::~InlineList()
	{
		this->Clear();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename T, std::size_t N>
	InlineList<T, N> &InlineList<T, N>::operator=(const InlineList &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (auto it = other.Begin(); it != other.End(); ++it)
			{
				static_cast<void>(this->Append(*it));
			}
		}

		return *this;
	}

	template <typename T, std::size_t N>
	InlineList<T, N> &InlineList<T, N>::operator=(InlineList &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			for (auto it = other.Begin(); it != other.End(); ++it)
			{
				static_cast<void>(this->Append(std::move(*it)));
			}

			other.Clear();
		}

		return *this;
	}

	template <typename T, std::size_t N>
	std::expected<typename InlineList<T, N>::NodeType *, Error> InlineList<T, N>::AllocateNode()
	{
		if (m_FreeList != nullptr)
		{
			Slot *slot = m_FreeList;
			m_FreeList = slot->next;

			return reinterpret_cast<NodeType *>(slot->storage);
		}

		auto result = m_Allocator->Allocate(sizeof(NodeType), alignof(NodeType));

		if (!result)
		{
			return std::unexpected(result.error());
		}

		return static_cast<NodeType *>(*result);
	}

	template <typename T, std::size_t N>
	void InlineList<T, N>::DeallocateNode(NodeType *node)
	{
		if (IsInline(node))
		{
			Slot *slot = reinterpret_cast<Slot *>(node);
			slot->next = m_FreeList;
			m_FreeList = slot;
		}
		else
		{
			m_Allocator->Deallocate(node, sizeof(NodeType));
		}
	}

	template <typename T, std::size_t N>
	typename InlineList<T, N>::SizeType InlineList<T, N>::InlineCapacity() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	bool InlineList<T, N>::IsInline(NodeType *node) const
	{
		auto addr = reinterpret_cast<const std::byte *>(node);
		auto start = reinterpret_cast<const std::byte *>(&m_Storage[0]);
		auto end = reinterpret_cast<const std::byte *>(&m_Storage[N]);

		return addr >= start && addr < end;
	}
} // namespace mlsl

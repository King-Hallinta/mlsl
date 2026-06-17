// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>
#include <utility>

namespace mlsl
{
	template <typename T, std::size_t N>
	ObjectPool<T, N>::ObjectPool()
	{
		for (std::size_t i = 0; i < N - 1; ++i)
		{
			m_Storage[i].next = &m_Storage[i + 1];
		}

		m_Storage[N - 1].next = nullptr;
		m_FreeList = &m_Storage[0];
	}

	template <typename T, std::size_t N>
	ObjectPool<T, N>::~ObjectPool()
	{
	}

	template <typename T, std::size_t N>
	template <typename... Args>
	std::expected<typename ObjectPool<T, N>::Pointer, Error> ObjectPool<T, N>::Acquire(Args &&...args)
	{
		if (m_FreeList == nullptr)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		Slot *slot = m_FreeList;
		m_FreeList = slot->next;
		--m_Available;

		return new (slot->storage) T(std::forward<Args>(args)...);
	}

	template <typename T, std::size_t N>
	void ObjectPool<T, N>::Release(Pointer ptr)
	{
		if (ptr == nullptr)
		{
			return;
		}

		ptr->~T();

		Slot *slot = reinterpret_cast<Slot *>(ptr);
		slot->next = m_FreeList;
		m_FreeList = slot;
		++m_Available;
	}

	template <typename T, std::size_t N>
	typename ObjectPool<T, N>::SizeType ObjectPool<T, N>::Available() const
	{
		return m_Available;
	}

	template <typename T, std::size_t N>
	typename ObjectPool<T, N>::SizeType ObjectPool<T, N>::Capacity() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	bool ObjectPool<T, N>::Owns(Pointer ptr) const
	{
		auto addr = reinterpret_cast<const std::byte *>(ptr);
		auto start = reinterpret_cast<const std::byte *>(&m_Storage[0]);
		auto end = reinterpret_cast<const std::byte *>(&m_Storage[N]);

		return addr >= start && addr < end;
	}
} // namespace mlsl

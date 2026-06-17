// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename T>
	Queue<T>::Queue()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename T>
	Queue<T>::Queue(Allocator *allocator)
		: m_Allocator(allocator), m_OwnsAllocator(false)
	{
	}

	template <typename T>
	Queue<T>::Queue(const Queue &other)
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(other.GetStorage()[other.MapIndex(i)]));
		}
	}

	template <typename T>
	Queue<T>::Queue(Queue &&other) noexcept
		: m_Storage(other.m_Storage), m_Capacity(other.m_Capacity), m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		this->m_Head = other.m_Head;
		this->m_Size = other.m_Size;

		other.m_Storage = nullptr;
		other.m_Capacity = 0;
		other.m_Head = 0;
		other.m_Size = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
	}

	template <typename T>
	Queue<T>::~Queue()
	{
		this->Clear();

		if (m_Storage)
		{
			m_Allocator->Deallocate(m_Storage, m_Capacity * sizeof(T));
		}

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename T>
	Queue<T> &Queue<T>::operator=(const Queue &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(other.GetStorage()[other.MapIndex(i)]));
			}
		}

		return *this;
	}

	template <typename T>
	Queue<T> &Queue<T>::operator=(Queue &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			if (m_Storage)
			{
				m_Allocator->Deallocate(m_Storage, m_Capacity * sizeof(T));
			}

			if (m_OwnsAllocator)
			{
				delete m_Allocator;
			}

			m_Storage = other.m_Storage;
			m_Capacity = other.m_Capacity;
			this->m_Head = other.m_Head;
			this->m_Size = other.m_Size;
			m_Allocator = other.m_Allocator;
			m_OwnsAllocator = other.m_OwnsAllocator;

			other.m_Storage = nullptr;
			other.m_Capacity = 0;
			other.m_Head = 0;
			other.m_Size = 0;
			other.m_Allocator = nullptr;
			other.m_OwnsAllocator = false;
		}

		return *this;
	}

	template <typename T>
	T *Queue<T>::GetStorage()
	{
		return m_Storage;
	}

	template <typename T>
	const T *Queue<T>::GetStorage() const
	{
		return m_Storage;
	}

	template <typename T>
	typename Queue<T>::SizeType Queue<T>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename T>
	std::expected<void, Error> Queue<T>::EnsureCapacity(SizeType required)
	{
		if (required <= m_Capacity)
		{
			return {};
		}

		SizeType newCapacity = m_Capacity == 0 ? 8 : m_Capacity * 2;

		while (newCapacity < required)
		{
			newCapacity *= 2;
		}

		return Grow(newCapacity);
	}

	template <typename T>
	typename Queue<T>::SizeType Queue<T>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename T>
	std::expected<void, Error> Queue<T>::Reserve(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Grow(capacity);
	}

	template <typename T>
	std::expected<void, Error> Queue<T>::Grow(SizeType newCapacity)
	{
		auto result = m_Allocator->Allocate(newCapacity * sizeof(T), alignof(T));

		if (!result)
		{
			return std::unexpected(result.error());
		}

		T *newStorage = static_cast<T *>(*result);

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newStorage[i]) T(std::move(GetStorage()[this->MapIndex(i)]));
			GetStorage()[this->MapIndex(i)].~T();
		}

		if (m_Storage)
		{
			m_Allocator->Deallocate(m_Storage, m_Capacity * sizeof(T));
		}

		m_Storage = newStorage;
		m_Capacity = newCapacity;
		this->m_Head = 0;

		return {};
	}
} // namespace mlsl

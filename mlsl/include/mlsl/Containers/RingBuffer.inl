// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>
#include <utility>

namespace mlsl
{
	template <typename T>
	RingBuffer<T>::RingBuffer()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename T>
	RingBuffer<T>::RingBuffer(Allocator *allocator)
	{
		if (allocator)
		{
			m_Allocator = allocator;
			m_OwnsAllocator = false;
		}
		else
		{
			m_Allocator = new DefaultAllocator();
			m_OwnsAllocator = true;
		}
	}

	template <typename T>
	RingBuffer<T>::RingBuffer(const RingBuffer &other)
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(other[i]));
		}
	}

	template <typename T>
	RingBuffer<T>::RingBuffer(RingBuffer &&other) noexcept
		: m_Storage(other.m_Storage), m_Capacity(other.m_Capacity), m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		this->m_Head = other.m_Head;
		this->m_Size = other.m_Size;

		other.m_Storage = nullptr;
		other.m_Capacity = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
		other.m_Head = 0;
		other.m_Size = 0;
	}

	template <typename T>
	RingBuffer<T>::~RingBuffer()
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
	RingBuffer<T> &RingBuffer<T>::operator=(const RingBuffer &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(other[i]));
			}
		}

		return *this;
	}

	template <typename T>
	RingBuffer<T> &RingBuffer<T>::operator=(RingBuffer &&other) noexcept
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
			m_Allocator = other.m_Allocator;
			m_OwnsAllocator = other.m_OwnsAllocator;
			this->m_Head = other.m_Head;
			this->m_Size = other.m_Size;

			other.m_Storage = nullptr;
			other.m_Capacity = 0;
			other.m_Allocator = nullptr;
			other.m_OwnsAllocator = false;
			other.m_Head = 0;
			other.m_Size = 0;
		}

		return *this;
	}

	template <typename T>
	T *RingBuffer<T>::GetStorage()
	{
		return m_Storage;
	}

	template <typename T>
	const T *RingBuffer<T>::GetStorage() const
	{
		return m_Storage;
	}

	template <typename T>
	typename RingBuffer<T>::SizeType RingBuffer<T>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename T>
	std::expected<void, Error> RingBuffer<T>::EnsureCapacity(SizeType required)
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
	typename RingBuffer<T>::SizeType RingBuffer<T>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename T>
	std::expected<void, Error> RingBuffer<T>::Reserve(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Grow(capacity);
	}

	template <typename T>
	std::expected<void, Error> RingBuffer<T>::Grow(SizeType newCapacity)
	{
		auto result = m_Allocator->Allocate(newCapacity * sizeof(T), alignof(T));

		if (not result)
		{
			return std::unexpected(result.error());
		}

		T *newStorage = static_cast<T *>(*result);

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newStorage[i]) T(std::move((*this)[i]));
			(*this)[i].~T();
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

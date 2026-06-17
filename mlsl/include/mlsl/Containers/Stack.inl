// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename T>
	Stack<T>::Stack()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename T>
	Stack<T>::Stack(Allocator *allocator)
		: m_Allocator(allocator), m_OwnsAllocator(false)
	{
	}

	template <typename T>
	Stack<T>::Stack(const Stack &other)
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(other.GetStorage()[i]));
		}
	}

	template <typename T>
	Stack<T>::Stack(Stack &&other) noexcept
		: m_Storage(other.m_Storage), m_Capacity(other.m_Capacity), m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		this->m_Size = other.m_Size;

		other.m_Storage = nullptr;
		other.m_Capacity = 0;
		other.m_Size = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
	}

	template <typename T>
	Stack<T>::~Stack()
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
	Stack<T> &Stack<T>::operator=(const Stack &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(other.GetStorage()[i]));
			}
		}

		return *this;
	}

	template <typename T>
	Stack<T> &Stack<T>::operator=(Stack &&other) noexcept
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
			this->m_Size = other.m_Size;
			m_Allocator = other.m_Allocator;
			m_OwnsAllocator = other.m_OwnsAllocator;

			other.m_Storage = nullptr;
			other.m_Capacity = 0;
			other.m_Size = 0;
			other.m_Allocator = nullptr;
			other.m_OwnsAllocator = false;
		}

		return *this;
	}

	template <typename T>
	T *Stack<T>::GetStorage()
	{
		return m_Storage;
	}

	template <typename T>
	const T *Stack<T>::GetStorage() const
	{
		return m_Storage;
	}

	template <typename T>
	typename Stack<T>::SizeType Stack<T>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename T>
	std::expected<void, Error> Stack<T>::EnsureCapacity(SizeType required)
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
	typename Stack<T>::SizeType Stack<T>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename T>
	std::expected<void, Error> Stack<T>::Reserve(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Grow(capacity);
	}

	template <typename T>
	std::expected<void, Error> Stack<T>::Grow(SizeType newCapacity)
	{
		auto result = m_Allocator->Allocate(newCapacity * sizeof(T), alignof(T));

		if (!result)
		{
			return std::unexpected(result.error());
		}

		T *newStorage = static_cast<T *>(*result);

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newStorage[i]) T(std::move(m_Storage[i]));
			m_Storage[i].~T();
		}

		if (m_Storage)
		{
			m_Allocator->Deallocate(m_Storage, m_Capacity * sizeof(T));
		}

		m_Storage = newStorage;
		m_Capacity = newCapacity;

		return {};
	}
} // namespace mlsl

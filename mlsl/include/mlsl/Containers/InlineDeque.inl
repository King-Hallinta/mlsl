// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename T, std::size_t N>
	InlineDeque<T, N>::InlineDeque()
		: m_Storage(reinterpret_cast<T *>(m_InlineStorage)), m_Capacity(N), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename T, std::size_t N>
	InlineDeque<T, N>::InlineDeque(Allocator *allocator)
		: m_Storage(reinterpret_cast<T *>(m_InlineStorage)), m_Capacity(N), m_Allocator(allocator), m_OwnsAllocator(false)
	{
	}

	template <typename T, std::size_t N>
	InlineDeque<T, N>::InlineDeque(const InlineDeque &other)
		: m_Storage(reinterpret_cast<T *>(m_InlineStorage)), m_Capacity(N), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->AddBack(other[i]));
		}
	}

	template <typename T, std::size_t N>
	InlineDeque<T, N>::InlineDeque(InlineDeque &&other) noexcept
		: m_Storage(reinterpret_cast<T *>(m_InlineStorage)), m_Capacity(N), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->AddBack(std::move(other[i])));
		}

		other.Clear();
	}

	template <typename T, std::size_t N>
	InlineDeque<T, N>::~InlineDeque()
	{
		this->Clear();

		if (!IsInline() && m_Storage)
		{
			m_Allocator->Deallocate(m_Storage, m_Capacity * sizeof(T));
		}

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename T, std::size_t N>
	InlineDeque<T, N> &InlineDeque<T, N>::operator=(const InlineDeque &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->AddBack(other[i]));
			}
		}

		return *this;
	}

	template <typename T, std::size_t N>
	InlineDeque<T, N> &InlineDeque<T, N>::operator=(InlineDeque &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->AddBack(std::move(other[i])));
			}

			other.Clear();
		}

		return *this;
	}

	template <typename T, std::size_t N>
	T *InlineDeque<T, N>::GetStorage()
	{
		return m_Storage;
	}

	template <typename T, std::size_t N>
	const T *InlineDeque<T, N>::GetStorage() const
	{
		return m_Storage;
	}

	template <typename T, std::size_t N>
	typename InlineDeque<T, N>::SizeType InlineDeque<T, N>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename T, std::size_t N>
	std::expected<void, Error> InlineDeque<T, N>::EnsureCapacity(SizeType required)
	{
		if (required <= m_Capacity)
		{
			return {};
		}

		SizeType newCapacity = m_Capacity * 2;

		while (newCapacity < required)
		{
			newCapacity *= 2;
		}

		return Grow(newCapacity);
	}

	template <typename T, std::size_t N>
	typename InlineDeque<T, N>::SizeType InlineDeque<T, N>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename T, std::size_t N>
	typename InlineDeque<T, N>::SizeType InlineDeque<T, N>::InlineCapacity() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	bool InlineDeque<T, N>::IsInline() const
	{
		return m_Storage == reinterpret_cast<const T *>(m_InlineStorage);
	}

	template <typename T, std::size_t N>
	std::expected<void, Error> InlineDeque<T, N>::Grow(SizeType newCapacity)
	{
		auto result = m_Allocator->Allocate(newCapacity * sizeof(T), alignof(T));

		if (!result)
		{
			return std::unexpected(result.error());
		}

		T *newStorage = static_cast<T *>(*result);

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newStorage[i]) T(std::move((*this)[i]));
			(*this)[i].~T();
		}

		if (!IsInline() && m_Storage)
		{
			m_Allocator->Deallocate(m_Storage, m_Capacity * sizeof(T));
		}

		m_Storage = newStorage;
		m_Capacity = newCapacity;
		this->m_Head = 0;

		return {};
	}
} // namespace mlsl

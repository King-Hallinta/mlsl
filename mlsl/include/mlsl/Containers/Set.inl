// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename LessT>
	Set<KeyT, LessT>::Set()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT>::Set(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT>::Set(LessT less)
		: Base(std::move(less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT>::Set(Allocator *allocator, LessT less)
		: Base(std::move(less)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT>::Set(const Set &other)
		: Base(other.m_Less), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT>::Set(Set &&other) noexcept
		: Base(std::move(other.m_Less)), m_Keys(other.m_Keys), m_Capacity(other.m_Capacity), m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		this->m_Size = other.m_Size;

		other.m_Keys = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT>::~Set()
	{
		this->Clear();
		ReleaseHeapKeys();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT> &Set<KeyT, LessT>::operator=(const Set &other)
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename LessT>
	Set<KeyT, LessT> &Set<KeyT, LessT>::operator=(Set &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			ReleaseHeapKeys();

			if (m_OwnsAllocator)
			{
				delete m_Allocator;
			}

			this->m_Less = std::move(other.m_Less);
			StealFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename LessT>
	KeyT *Set<KeyT, LessT>::GetKeys()
	{
		return m_Keys;
	}

	template <typename KeyT, typename LessT>
	const KeyT *Set<KeyT, LessT>::GetKeys() const
	{
		return m_Keys;
	}

	template <typename KeyT, typename LessT>
	typename Set<KeyT, LessT>::SizeType Set<KeyT, LessT>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename KeyT, typename LessT>
	std::expected<void, Error> Set<KeyT, LessT>::EnsureCapacity(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		SizeType newCapacity = m_Capacity == 0 ? 8 : m_Capacity * 2;

		while (newCapacity < capacity)
		{
			newCapacity *= 2;
		}

		return Grow(newCapacity);
	}

	template <typename KeyT, typename LessT>
	std::expected<KeyT *, Error> Set<KeyT, LessT>::AllocateKeys(SizeType capacity)
	{
		auto allocation = m_Allocator->Allocate(capacity * sizeof(KeyT), alignof(KeyT));

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		return static_cast<KeyT *>(*allocation);
	}

	template <typename KeyT, typename LessT>
	std::expected<void, Error> Set<KeyT, LessT>::Grow(SizeType capacity)
	{
		auto allocation = AllocateKeys(capacity);

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		KeyT *oldKeys = m_Keys;
		SizeType oldCapacity = m_Capacity;
		KeyT *newKeys = *allocation;

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newKeys[i]) KeyT {std::move(oldKeys[i])};
			oldKeys[i].~KeyT();
		}

		if (oldKeys != nullptr)
		{
			m_Allocator->Deallocate(oldKeys, oldCapacity * sizeof(KeyT));
		}

		m_Keys = newKeys;
		m_Capacity = capacity;

		return {};
	}

	template <typename KeyT, typename LessT>
	void Set<KeyT, LessT>::ReleaseHeapKeys()
	{
		if (m_Keys != nullptr and m_Allocator != nullptr)
		{
			m_Allocator->Deallocate(m_Keys, m_Capacity * sizeof(KeyT));
		}

		m_Keys = nullptr;
		m_Capacity = 0;
	}

	template <typename KeyT, typename LessT>
	void Set<KeyT, LessT>::CopyFrom(const Set &other)
	{
		if (other.m_Size == 0)
		{
			return;
		}

		auto result = this->Reserve(other.m_Size);

		if (not result)
		{
			return;
		}

		const KeyT *source = other.m_Keys;

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&m_Keys[i]) KeyT {source[i]};
		}

		this->m_Size = other.m_Size;
	}

	template <typename KeyT, typename LessT>
	void Set<KeyT, LessT>::StealFrom(Set &other)
	{
		m_Keys = other.m_Keys;
		this->m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		m_Allocator = other.m_Allocator;
		m_OwnsAllocator = other.m_OwnsAllocator;

		other.m_Keys = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
	}
} // namespace mlsl

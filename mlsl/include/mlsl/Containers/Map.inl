// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT>::Map()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT>::Map(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT>::Map(LessT less)
		: Base(std::move(less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT>::Map(Allocator *allocator, LessT less)
		: Base(std::move(less)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT>::Map(const Map &other)
		: Base(other.m_Less), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT>::Map(Map &&other) noexcept
		: Base(std::move(other.m_Less)), m_Entries(other.m_Entries), m_Capacity(other.m_Capacity), m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		this->m_Size = other.m_Size;

		other.m_Entries = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT>::~Map()
	{
		this->Clear();
		ReleaseHeapEntries();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT> &Map<KeyT, ValueT, LessT>::operator=(const Map &other)
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	Map<KeyT, ValueT, LessT> &Map<KeyT, ValueT, LessT>::operator=(Map &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			ReleaseHeapEntries();

			if (m_OwnsAllocator)
			{
				delete m_Allocator;
			}

			this->m_Less = std::move(other.m_Less);
			StealFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	typename Map<KeyT, ValueT, LessT>::Entry *Map<KeyT, ValueT, LessT>::GetEntries()
	{
		return m_Entries;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	const typename Map<KeyT, ValueT, LessT>::Entry *Map<KeyT, ValueT, LessT>::GetEntries() const
	{
		return m_Entries;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	typename Map<KeyT, ValueT, LessT>::SizeType Map<KeyT, ValueT, LessT>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	std::expected<void, Error> Map<KeyT, ValueT, LessT>::EnsureCapacity(SizeType capacity)
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

	template <typename KeyT, typename ValueT, typename LessT>
	std::expected<typename Map<KeyT, ValueT, LessT>::Entry *, Error> Map<KeyT, ValueT, LessT>::AllocateEntries(SizeType capacity)
	{
		auto allocation = m_Allocator->Allocate(capacity * sizeof(Entry), alignof(Entry));

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		return static_cast<Entry *>(*allocation);
	}

	template <typename KeyT, typename ValueT, typename LessT>
	std::expected<void, Error> Map<KeyT, ValueT, LessT>::Grow(SizeType capacity)
	{
		auto allocation = AllocateEntries(capacity);

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		Entry *oldEntries = m_Entries;
		SizeType oldCapacity = m_Capacity;
		Entry *newEntries = *allocation;

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newEntries[i]) Entry {std::move(oldEntries[i].key), std::move(oldEntries[i].value)};
			oldEntries[i].~Entry();
		}

		if (oldEntries != nullptr)
		{
			m_Allocator->Deallocate(oldEntries, oldCapacity * sizeof(Entry));
		}

		m_Entries = newEntries;
		m_Capacity = capacity;

		return {};
	}

	template <typename KeyT, typename ValueT, typename LessT>
	void Map<KeyT, ValueT, LessT>::ReleaseHeapEntries()
	{
		if (m_Entries != nullptr and m_Allocator != nullptr)
		{
			m_Allocator->Deallocate(m_Entries, m_Capacity * sizeof(Entry));
		}

		m_Entries = nullptr;
		m_Capacity = 0;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	void Map<KeyT, ValueT, LessT>::CopyFrom(const Map &other)
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

		const Entry *source = other.m_Entries;

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&m_Entries[i]) Entry {source[i].key, source[i].value};
		}

		this->m_Size = other.m_Size;
	}

	template <typename KeyT, typename ValueT, typename LessT>
	void Map<KeyT, ValueT, LessT>::StealFrom(Map &other)
	{
		m_Entries = other.m_Entries;
		this->m_Size = other.m_Size;
		m_Capacity = other.m_Capacity;
		m_Allocator = other.m_Allocator;
		m_OwnsAllocator = other.m_OwnsAllocator;

		other.m_Entries = nullptr;
		other.m_Size = 0;
		other.m_Capacity = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
	}
} // namespace mlsl

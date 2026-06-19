// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(LessT less)
		: Base(std::move(less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(Allocator *allocator, LessT less)
		: Base(std::move(less)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(const InlineMap &other)
		: Base(other.m_Less), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(InlineMap &&other) noexcept
		: Base(std::move(other.m_Less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		MoveFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::~InlineMap()
	{
		this->Clear();
		ReleaseHeapEntries();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT> &InlineMap<KeyT, ValueT, N, LessT>::operator=(const InlineMap &other)
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT> &InlineMap<KeyT, ValueT, N, LessT>::operator=(InlineMap &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::SizeType InlineMap<KeyT, ValueT, N, LessT>::InlineCapacity() const
	{
		return N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::Entry *InlineMap<KeyT, ValueT, N, LessT>::GetEntries()
	{
		return m_Entries;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	const typename InlineMap<KeyT, ValueT, N, LessT>::Entry *InlineMap<KeyT, ValueT, N, LessT>::GetEntries() const
	{
		return m_Entries;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::SizeType InlineMap<KeyT, ValueT, N, LessT>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::EnsureCapacity(SizeType capacity)
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

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::Entry *InlineMap<KeyT, ValueT, N, LessT>::InlineEntries()
	{
		return reinterpret_cast<Entry *>(m_InlineStorage);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	const typename InlineMap<KeyT, ValueT, N, LessT>::Entry *InlineMap<KeyT, ValueT, N, LessT>::InlineEntries() const
	{
		return reinterpret_cast<const Entry *>(m_InlineStorage);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool InlineMap<KeyT, ValueT, N, LessT>::IsInline() const
	{
		return m_Entries == InlineEntries();
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<typename InlineMap<KeyT, ValueT, N, LessT>::Entry *, Error> InlineMap<KeyT, ValueT, N, LessT>::AllocateEntries(SizeType capacity)
	{
		auto allocation = m_Allocator->Allocate(capacity * sizeof(Entry), alignof(Entry));

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		return static_cast<Entry *>(*allocation);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::Grow(SizeType capacity)
	{
		auto allocation = AllocateEntries(capacity);

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		Entry *oldEntries = m_Entries;
		SizeType oldCapacity = m_Capacity;
		bool wasInline = IsInline();
		Entry *newEntries = *allocation;

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newEntries[i]) Entry {std::move(oldEntries[i].key), std::move(oldEntries[i].value)};
			oldEntries[i].~Entry();
		}

		if (not wasInline)
		{
			m_Allocator->Deallocate(oldEntries, oldCapacity * sizeof(Entry));
		}

		m_Entries = newEntries;
		m_Capacity = capacity;

		return {};
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void InlineMap<KeyT, ValueT, N, LessT>::ReleaseHeapEntries()
	{
		if (not IsInline() and m_Entries != nullptr and m_Allocator != nullptr)
		{
			m_Allocator->Deallocate(m_Entries, m_Capacity * sizeof(Entry));
		}

		m_Entries = InlineEntries();
		m_Capacity = N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void InlineMap<KeyT, ValueT, N, LessT>::CopyFrom(const InlineMap &other)
	{
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

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void InlineMap<KeyT, ValueT, N, LessT>::MoveFrom(InlineMap &other)
	{
		auto result = this->Reserve(other.m_Size);

		if (not result)
		{
			return;
		}

		Entry *source = other.m_Entries;

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&m_Entries[i]) Entry {std::move(source[i].key), std::move(source[i].value)};
		}

		this->m_Size = other.m_Size;
		other.Clear();
	}
} // namespace mlsl

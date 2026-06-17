// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

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
		: m_Less(std::move(less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(Allocator *allocator, LessT less)
		: m_Less(std::move(less)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(const InlineMap &other)
		: m_Less(other.m_Less), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::InlineMap(InlineMap &&other) noexcept
		: m_Less(std::move(other.m_Less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		MoveFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT>::~InlineMap()
	{
		Clear();
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
			Clear();
			m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	InlineMap<KeyT, ValueT, N, LessT> &InlineMap<KeyT, ValueT, N, LessT>::operator=(InlineMap &&other) noexcept
	{
		if (this != &other)
		{
			Clear();
			m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::SizeType InlineMap<KeyT, ValueT, N, LessT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::SizeType InlineMap<KeyT, ValueT, N, LessT>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::SizeType InlineMap<KeyT, ValueT, N, LessT>::InlineCapacity() const
	{
		return N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool InlineMap<KeyT, ValueT, N, LessT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void InlineMap<KeyT, ValueT, N, LessT>::Clear()
	{
		DestroyEntries();
		m_Size = 0;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::Reserve(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Grow(capacity);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool InlineMap<KeyT, ValueT, N, LessT>::Contains(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		return index < m_Size and KeysEqual(m_Entries[index].key, key);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<typename InlineMap<KeyT, ValueT, N, LessT>::Pointer, Error> InlineMap<KeyT, ValueT, N, LessT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(m_Entries[index].key, key))
		{
			return &m_Entries[index].value;
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<typename InlineMap<KeyT, ValueT, N, LessT>::ConstPointer, Error> InlineMap<KeyT, ValueT, N, LessT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(m_Entries[index].key, key))
		{
			return &m_Entries[index].value;
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::Add(const KeyT &key, const ValueT &value)
	{
		return AddInternal(key, value);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::Add(const KeyT &key, ValueT &&value)
	{
		return AddInternal(key, std::move(value));
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::Add(KeyT &&key, const ValueT &value)
	{
		return AddInternal(std::move(key), value);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::Add(KeyT &&key, ValueT &&value)
	{
		return AddInternal(std::move(key), std::move(value));
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	bool InlineMap<KeyT, ValueT, N, LessT>::Remove(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index >= m_Size or not KeysEqual(m_Entries[index].key, key))
		{
			return false;
		}

		m_Entries[index].~Entry();

		for (SizeType i = index; i + 1 < m_Size; ++i)
		{
			new (&m_Entries[i]) Entry {std::move(m_Entries[i + 1].key), std::move(m_Entries[i + 1].value)};
			m_Entries[i + 1].~Entry();
		}

		--m_Size;

		return true;
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
	bool InlineMap<KeyT, ValueT, N, LessT>::KeysEqual(const KeyT &left, const KeyT &right) const
	{
		return not m_Less(left, right) and not m_Less(right, left);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	typename InlineMap<KeyT, ValueT, N, LessT>::SizeType InlineMap<KeyT, ValueT, N, LessT>::FindIndex(const KeyT &key) const
	{
		SizeType first = 0;
		SizeType count = m_Size;

		while (count > 0)
		{
			SizeType step = count / 2;
			SizeType middle = first + step;

			if (m_Less(m_Entries[middle].key, key))
			{
				first = middle + 1;
				count -= step + 1;
			}
			else
			{
				count = step;
			}
		}

		return first;
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
	template <typename KeyArg, typename ValueArg>
	std::expected<void, Error> InlineMap<KeyT, ValueT, N, LessT>::AddInternal(KeyArg &&key, ValueArg &&value)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(m_Entries[index].key, key))
		{
			return std::unexpected(Error {ErrorType::AlreadyExists});
		}

		auto result = EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		if (index < m_Size)
		{
			new (&m_Entries[m_Size]) Entry {std::move(m_Entries[m_Size - 1].key), std::move(m_Entries[m_Size - 1].value)};

			for (SizeType i = m_Size - 1; i > index; --i)
			{
				m_Entries[i].~Entry();
				new (&m_Entries[i]) Entry {std::move(m_Entries[i - 1].key), std::move(m_Entries[i - 1].value)};
			}

			m_Entries[index].~Entry();
		}

		new (&m_Entries[index]) Entry {std::forward<KeyArg>(key), std::forward<ValueArg>(value)};
		++m_Size;

		return {};
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void InlineMap<KeyT, ValueT, N, LessT>::DestroyEntries()
	{
		for (SizeType i = 0; i < m_Size; ++i)
		{
			m_Entries[i].~Entry();
		}
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

		for (SizeType i = 0; i < m_Size; ++i)
		{
			new (&newEntries[i]) Entry {std::move(m_Entries[i].key), std::move(m_Entries[i].value)};
			m_Entries[i].~Entry();
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
	void InlineMap<KeyT, ValueT, N, LessT>::CopyFrom(const InlineMap &other)
	{
		static_cast<void>(Reserve(other.m_Size));

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(Add(other.m_Entries[i].key, other.m_Entries[i].value));
		}
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename LessT>
	void InlineMap<KeyT, ValueT, N, LessT>::MoveFrom(InlineMap &other)
	{
		auto result = Reserve(other.m_Size);

		if (not result)
		{
			return;
		}

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(Add(std::move(other.m_Entries[i].key), std::move(other.m_Entries[i].value)));
		}

		other.Clear();
	}
} // namespace mlsl

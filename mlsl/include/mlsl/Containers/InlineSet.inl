// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <new>

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(LessT less)
		: m_Less(std::move(less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(Allocator *allocator, LessT less)
		: m_Less(std::move(less)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(const InlineSet &other)
		: m_Less(other.m_Less), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(InlineSet &&other) noexcept
		: m_Less(std::move(other.m_Less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		MoveFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::~InlineSet()
	{
		Clear();
		ReleaseHeapKeys();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT> &InlineSet<KeyT, N, LessT>::operator=(const InlineSet &other)
	{
		if (this != &other)
		{
			Clear();
			m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT> &InlineSet<KeyT, N, LessT>::operator=(InlineSet &&other) noexcept
	{
		if (this != &other)
		{
			Clear();
			m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename InlineSet<KeyT, N, LessT>::SizeType InlineSet<KeyT, N, LessT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename InlineSet<KeyT, N, LessT>::SizeType InlineSet<KeyT, N, LessT>::Capacity() const
	{
		return m_Capacity;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename InlineSet<KeyT, N, LessT>::SizeType InlineSet<KeyT, N, LessT>::InlineCapacity() const
	{
		return N;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool InlineSet<KeyT, N, LessT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void InlineSet<KeyT, N, LessT>::Clear()
	{
		DestroyKeys();
		m_Size = 0;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineSet<KeyT, N, LessT>::Reserve(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Grow(capacity);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool InlineSet<KeyT, N, LessT>::Contains(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		return index < m_Size and KeysEqual(m_Keys[index], key);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<typename InlineSet<KeyT, N, LessT>::Pointer, Error> InlineSet<KeyT, N, LessT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(m_Keys[index], key))
		{
			return &m_Keys[index];
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<typename InlineSet<KeyT, N, LessT>::ConstPointer, Error> InlineSet<KeyT, N, LessT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(m_Keys[index], key))
		{
			return &m_Keys[index];
		}

		return std::unexpected(Error {ErrorType::NotFound});
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineSet<KeyT, N, LessT>::Add(const KeyT &key)
	{
		return AddInternal(key);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineSet<KeyT, N, LessT>::Add(KeyT &&key)
	{
		return AddInternal(std::move(key));
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool InlineSet<KeyT, N, LessT>::Remove(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index >= m_Size or not KeysEqual(m_Keys[index], key))
		{
			return false;
		}

		m_Keys[index].~KeyT();

		for (SizeType i = index; i + 1 < m_Size; ++i)
		{
			new (&m_Keys[i]) KeyT(std::move(m_Keys[i + 1]));
			m_Keys[i + 1].~KeyT();
		}

		--m_Size;

		return true;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	KeyT *InlineSet<KeyT, N, LessT>::InlineKeys()
	{
		return reinterpret_cast<KeyT *>(m_InlineStorage);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	const KeyT *InlineSet<KeyT, N, LessT>::InlineKeys() const
	{
		return reinterpret_cast<const KeyT *>(m_InlineStorage);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool InlineSet<KeyT, N, LessT>::IsInline() const
	{
		return m_Keys == InlineKeys();
	}

	template <typename KeyT, std::size_t N, typename LessT>
	bool InlineSet<KeyT, N, LessT>::KeysEqual(const KeyT &left, const KeyT &right) const
	{
		return not m_Less(left, right) and not m_Less(right, left);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename InlineSet<KeyT, N, LessT>::SizeType InlineSet<KeyT, N, LessT>::FindIndex(const KeyT &key) const
	{
		SizeType first = 0;
		SizeType count = m_Size;

		while (count > 0)
		{
			SizeType step = count / 2;
			SizeType middle = first + step;

			if (m_Less(m_Keys[middle], key))
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

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineSet<KeyT, N, LessT>::EnsureCapacity(SizeType capacity)
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

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<KeyT *, Error> InlineSet<KeyT, N, LessT>::AllocateKeys(SizeType capacity)
	{
		auto allocation = m_Allocator->Allocate(capacity * sizeof(KeyT), alignof(KeyT));

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		return static_cast<KeyT *>(*allocation);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	template <typename KeyArg>
	std::expected<void, Error> InlineSet<KeyT, N, LessT>::AddInternal(KeyArg &&key)
	{
		SizeType index = FindIndex(key);

		if (index < m_Size and KeysEqual(m_Keys[index], key))
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
			new (&m_Keys[m_Size]) KeyT(std::move(m_Keys[m_Size - 1]));

			for (SizeType i = m_Size - 1; i > index; --i)
			{
				m_Keys[i].~KeyT();
				new (&m_Keys[i]) KeyT(std::move(m_Keys[i - 1]));
			}

			m_Keys[index].~KeyT();
		}

		new (&m_Keys[index]) KeyT(std::forward<KeyArg>(key));
		++m_Size;

		return {};
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void InlineSet<KeyT, N, LessT>::DestroyKeys()
	{
		for (SizeType i = 0; i < m_Size; ++i)
		{
			m_Keys[i].~KeyT();
		}
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void InlineSet<KeyT, N, LessT>::ReleaseHeapKeys()
	{
		if (not IsInline() and m_Keys != nullptr and m_Allocator != nullptr)
		{
			m_Allocator->Deallocate(m_Keys, m_Capacity * sizeof(KeyT));
		}

		m_Keys = InlineKeys();
		m_Capacity = N;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	std::expected<void, Error> InlineSet<KeyT, N, LessT>::Grow(SizeType capacity)
	{
		auto allocation = AllocateKeys(capacity);

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		KeyT *oldKeys = m_Keys;
		SizeType oldCapacity = m_Capacity;
		bool wasInline = IsInline();
		KeyT *newKeys = *allocation;

		for (SizeType i = 0; i < m_Size; ++i)
		{
			new (&newKeys[i]) KeyT(std::move(m_Keys[i]));
			m_Keys[i].~KeyT();
		}

		if (not wasInline)
		{
			m_Allocator->Deallocate(oldKeys, oldCapacity * sizeof(KeyT));
		}

		m_Keys = newKeys;
		m_Capacity = capacity;

		return {};
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void InlineSet<KeyT, N, LessT>::CopyFrom(const InlineSet &other)
	{
		static_cast<void>(Reserve(other.m_Size));

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(Add(other.m_Keys[i]));
		}
	}

	template <typename KeyT, std::size_t N, typename LessT>
	void InlineSet<KeyT, N, LessT>::MoveFrom(InlineSet &other)
	{
		auto result = Reserve(other.m_Size);

		if (not result)
		{
			return;
		}

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(Add(std::move(other.m_Keys[i])));
		}

		other.Clear();
	}
} // namespace mlsl

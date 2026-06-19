// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

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
		: Base(std::move(less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(Allocator *allocator, LessT less)
		: Base(std::move(less)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(const InlineSet &other)
		: Base(other.m_Less), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::InlineSet(InlineSet &&other) noexcept
		: Base(std::move(other.m_Less)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		MoveFrom(other);
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT>::~InlineSet()
	{
		this->Clear();
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
			this->Clear();
			this->m_Less = other.m_Less;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	InlineSet<KeyT, N, LessT> &InlineSet<KeyT, N, LessT>::operator=(InlineSet &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Less = std::move(other.m_Less);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename InlineSet<KeyT, N, LessT>::SizeType InlineSet<KeyT, N, LessT>::InlineCapacity() const
	{
		return N;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	KeyT *InlineSet<KeyT, N, LessT>::GetKeys()
	{
		return m_Keys;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	const KeyT *InlineSet<KeyT, N, LessT>::GetKeys() const
	{
		return m_Keys;
	}

	template <typename KeyT, std::size_t N, typename LessT>
	typename InlineSet<KeyT, N, LessT>::SizeType InlineSet<KeyT, N, LessT>::GetCapacity() const
	{
		return m_Capacity;
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

		for (SizeType i = 0; i < this->m_Size; ++i)
		{
			new (&newKeys[i]) KeyT {std::move(oldKeys[i])};
			oldKeys[i].~KeyT();
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
	void InlineSet<KeyT, N, LessT>::CopyFrom(const InlineSet &other)
	{
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

	template <typename KeyT, std::size_t N, typename LessT>
	void InlineSet<KeyT, N, LessT>::MoveFrom(InlineSet &other)
	{
		auto result = this->Reserve(other.m_Size);

		if (not result)
		{
			return;
		}

		KeyT *source = other.m_Keys;

		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			new (&m_Keys[i]) KeyT {std::move(source[i])};
		}

		this->m_Size = other.m_Size;
		other.Clear();
	}
} // namespace mlsl

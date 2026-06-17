// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT>::HashSet()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT>::HashSet(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT>::HashSet(HashT hash, EqualT equal)
		: Base(std::move(hash), std::move(equal)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT>::HashSet(Allocator *allocator, HashT hash, EqualT equal)
		: Base(std::move(hash), std::move(equal)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT>::HashSet(const HashSet &other)
		: Base(other.m_Hash, other.m_Equal), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT>::HashSet(HashSet &&other) noexcept
		: Base(std::move(other.m_Hash), std::move(other.m_Equal)), m_Slots(other.m_Slots), m_SlotCount(other.m_SlotCount), m_Capacity(other.m_Capacity), m_Allocator(other.m_Allocator), m_OwnsAllocator(other.m_OwnsAllocator)
	{
		this->m_Size = other.m_Size;
		this->m_Deleted = other.m_Deleted;

		other.m_Slots = nullptr;
		other.m_SlotCount = 0;
		other.m_Capacity = 0;
		other.m_Allocator = nullptr;
		other.m_OwnsAllocator = false;
		other.ResetSize();
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT>::~HashSet()
	{
		this->Clear();
		ReleaseSlots();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT> &HashSet<KeyT, HashT, EqualT>::operator=(const HashSet &other)
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Hash = other.m_Hash;
			this->m_Equal = other.m_Equal;
			CopyFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	HashSet<KeyT, HashT, EqualT> &HashSet<KeyT, HashT, EqualT>::operator=(HashSet &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			ReleaseSlots();

			if (m_OwnsAllocator)
			{
				delete m_Allocator;
			}

			this->m_Hash = std::move(other.m_Hash);
			this->m_Equal = std::move(other.m_Equal);
			this->m_Size = other.m_Size;
			this->m_Deleted = other.m_Deleted;
			m_Slots = other.m_Slots;
			m_SlotCount = other.m_SlotCount;
			m_Capacity = other.m_Capacity;
			m_Allocator = other.m_Allocator;
			m_OwnsAllocator = other.m_OwnsAllocator;

			other.m_Slots = nullptr;
			other.m_SlotCount = 0;
			other.m_Capacity = 0;
			other.m_Allocator = nullptr;
			other.m_OwnsAllocator = false;
			other.ResetSize();
		}

		return *this;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	typename HashSet<KeyT, HashT, EqualT>::Slot *HashSet<KeyT, HashT, EqualT>::GetSlots()
	{
		return m_Slots;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	const typename HashSet<KeyT, HashT, EqualT>::Slot *HashSet<KeyT, HashT, EqualT>::GetSlots() const
	{
		return m_Slots;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	typename HashSet<KeyT, HashT, EqualT>::SizeType HashSet<KeyT, HashT, EqualT>::GetSlotCount() const
	{
		return m_SlotCount;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	typename HashSet<KeyT, HashT, EqualT>::SizeType HashSet<KeyT, HashT, EqualT>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	std::expected<void, Error> HashSet<KeyT, HashT, EqualT>::EnsureCapacity(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Rebuild(Base::CalculateSlotCount(capacity, true));
	}

	template <typename KeyT, typename HashT, typename EqualT>
	std::expected<typename HashSet<KeyT, HashT, EqualT>::Slot *, Error> HashSet<KeyT, HashT, EqualT>::AllocateSlots(SizeType slotCount)
	{
		auto allocation = m_Allocator->Allocate(slotCount * sizeof(Slot), alignof(Slot));

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		Slot *slots = static_cast<Slot *>(*allocation);

		for (SizeType i = 0; i < slotCount; ++i)
		{
			new (&slots[i]) Slot();
		}

		return slots;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	void HashSet<KeyT, HashT, EqualT>::DestroySlotArray(Slot *slots, SizeType slotCount)
	{
		for (SizeType i = 0; i < slotCount; ++i)
		{
			slots[i].~Slot();
		}
	}

	template <typename KeyT, typename HashT, typename EqualT>
	void HashSet<KeyT, HashT, EqualT>::ReleaseSlots()
	{
		if (m_Slots != nullptr and m_Allocator != nullptr)
		{
			DestroySlotArray(m_Slots, m_SlotCount);
			m_Allocator->Deallocate(m_Slots, m_SlotCount * sizeof(Slot));
		}

		m_Slots = nullptr;
		m_SlotCount = 0;
		m_Capacity = 0;
	}

	template <typename KeyT, typename HashT, typename EqualT>
	std::expected<void, Error> HashSet<KeyT, HashT, EqualT>::Rebuild(SizeType slotCount)
	{
		auto allocation = AllocateSlots(slotCount);

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		Slot *oldSlots = m_Slots;
		SizeType oldSlotCount = m_SlotCount;
		Slot *newSlots = *allocation;

		this->MoveSlots(newSlots, slotCount);

		if (oldSlots != nullptr)
		{
			DestroySlotArray(oldSlots, oldSlotCount);
			m_Allocator->Deallocate(oldSlots, oldSlotCount * sizeof(Slot));
		}

		m_Slots = newSlots;
		m_SlotCount = slotCount;
		m_Capacity = Base::CalculateCapacity(slotCount);

		return {};
	}

	template <typename KeyT, typename HashT, typename EqualT>
	void HashSet<KeyT, HashT, EqualT>::CopyFrom(const HashSet &other)
	{
		static_cast<void>(this->Reserve(other.Size()));

		for (SizeType i = 0; i < other.m_SlotCount; ++i)
		{
			if (other.m_Slots[i].status == Base::SlotStatus::Occupied)
			{
				static_cast<void>(this->Add(*Base::KeyFrom(other.m_Slots[i])));
			}
		}
	}
} // namespace mlsl

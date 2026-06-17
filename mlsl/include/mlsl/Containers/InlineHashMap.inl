// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::InlineHashMap()
		: m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::InlineHashMap(Allocator *allocator)
		: m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::InlineHashMap(HashT hash, EqualT equal)
		: Base(std::move(hash), std::move(equal)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::InlineHashMap(Allocator *allocator, HashT hash, EqualT equal)
		: Base(std::move(hash), std::move(equal)), m_Allocator(allocator == nullptr ? new DefaultAllocator() : allocator), m_OwnsAllocator(allocator == nullptr)
	{
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::InlineHashMap(const InlineHashMap &other)
		: Base(other.m_Hash, other.m_Equal), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		CopyFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::InlineHashMap(InlineHashMap &&other) noexcept
		: Base(std::move(other.m_Hash), std::move(other.m_Equal)), m_Allocator(new DefaultAllocator()), m_OwnsAllocator(true)
	{
		MoveFrom(other);
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::~InlineHashMap()
	{
		this->Clear();
		ReleaseHeapSlots();

		if (m_OwnsAllocator)
		{
			delete m_Allocator;
		}
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT> &InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::operator=(const InlineHashMap &other)
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

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	InlineHashMap<KeyT, ValueT, N, HashT, EqualT> &InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::operator=(InlineHashMap &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();
			this->m_Hash = std::move(other.m_Hash);
			this->m_Equal = std::move(other.m_Equal);
			MoveFrom(other);
		}

		return *this;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	typename InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::SizeType InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::InlineCapacity() const
	{
		return N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	typename InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::Slot *InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::GetSlots()
	{
		return m_Slots;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	const typename InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::Slot *InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::GetSlots() const
	{
		return m_Slots;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	typename InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::SizeType InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::GetSlotCount() const
	{
		return m_SlotCount;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	typename InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::SizeType InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::GetCapacity() const
	{
		return m_Capacity;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	std::expected<void, Error> InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::EnsureCapacity(SizeType capacity)
	{
		if (capacity <= m_Capacity)
		{
			return {};
		}

		return Rebuild(Base::CalculateSlotCount(capacity, true));
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	bool InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::IsInline() const
	{
		return m_Slots == m_InlineSlots;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	std::expected<typename InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::Slot *, Error> InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::AllocateSlots(SizeType slotCount)
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

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	void InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::DestroySlotArray(Slot *slots, SizeType slotCount)
	{
		for (SizeType i = 0; i < slotCount; ++i)
		{
			slots[i].~Slot();
		}
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	void InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::ReleaseHeapSlots()
	{
		if (not IsInline() and m_Slots != nullptr and m_Allocator != nullptr)
		{
			DestroySlotArray(m_Slots, m_SlotCount);
			m_Allocator->Deallocate(m_Slots, m_SlotCount * sizeof(Slot));
		}

		m_Slots = m_InlineSlots;
		m_SlotCount = InlineSlotCount;
		m_Capacity = N;
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	std::expected<void, Error> InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::Rebuild(SizeType slotCount)
	{
		auto allocation = AllocateSlots(slotCount);

		if (not allocation)
		{
			return std::unexpected(allocation.error());
		}

		Slot *oldSlots = m_Slots;
		SizeType oldSlotCount = m_SlotCount;
		bool wasInline = IsInline();
		Slot *newSlots = *allocation;

		this->MoveSlots(newSlots, slotCount);

		if (not wasInline and oldSlots != nullptr)
		{
			DestroySlotArray(oldSlots, oldSlotCount);
			m_Allocator->Deallocate(oldSlots, oldSlotCount * sizeof(Slot));
		}

		m_Slots = newSlots;
		m_SlotCount = slotCount;
		m_Capacity = Base::CalculateCapacity(slotCount);

		return {};
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	void InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::CopyFrom(const InlineHashMap &other)
	{
		static_cast<void>(this->Reserve(other.Size()));

		for (SizeType i = 0; i < other.GetSlotCount(); ++i)
		{
			if (other.m_Slots[i].status == Base::SlotStatus::Occupied)
			{
				static_cast<void>(this->Add(*Base::KeyFrom(other.m_Slots[i]), *Base::ValueFrom(other.m_Slots[i])));
			}
		}
	}

	template <typename KeyT, typename ValueT, std::size_t N, typename HashT, typename EqualT>
	void InlineHashMap<KeyT, ValueT, N, HashT, EqualT>::MoveFrom(InlineHashMap &other)
	{
		auto result = this->Reserve(other.Size());

		if (not result)
		{
			return;
		}

		for (SizeType i = 0; i < other.GetSlotCount(); ++i)
		{
			if (other.m_Slots[i].status == Base::SlotStatus::Occupied)
			{
				static_cast<void>(this->Add(std::move(*Base::KeyFrom(other.m_Slots[i])), std::move(*Base::ValueFrom(other.m_Slots[i]))));
			}
		}

		other.Clear();
	}
} // namespace mlsl

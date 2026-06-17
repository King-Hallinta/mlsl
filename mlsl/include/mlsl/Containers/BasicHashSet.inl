// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	BasicHashSet<KeyT, Derived, HashT, EqualT>::BasicHashSet(HashT hash, EqualT equal)
		: m_Hash(std::move(hash)), m_Equal(std::move(equal))
	{
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashSet<KeyT, Derived, HashT, EqualT>::SizeType BasicHashSet<KeyT, Derived, HashT, EqualT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashSet<KeyT, Derived, HashT, EqualT>::SizeType BasicHashSet<KeyT, Derived, HashT, EqualT>::Capacity() const
	{
		return AsDerived().GetCapacity();
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	bool BasicHashSet<KeyT, Derived, HashT, EqualT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	void BasicHashSet<KeyT, Derived, HashT, EqualT>::Clear()
	{
		ClearSlots(AsDerived().GetSlots(), AsDerived().GetSlotCount());
		m_Size = 0;
		m_Deleted = 0;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashSet<KeyT, Derived, HashT, EqualT>::Reserve(SizeType capacity)
	{
		return AsDerived().EnsureCapacity(capacity);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	bool BasicHashSet<KeyT, Derived, HashT, EqualT>::Contains(const KeyT &key) const
	{
		return FindIndex(key) != Npos;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	std::expected<typename BasicHashSet<KeyT, Derived, HashT, EqualT>::Pointer, Error> BasicHashSet<KeyT, Derived, HashT, EqualT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index == Npos)
		{
			return std::unexpected(Error {ErrorType::NotFound});
		}

		return KeyFrom(AsDerived().GetSlots()[index]);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	std::expected<typename BasicHashSet<KeyT, Derived, HashT, EqualT>::ConstPointer, Error> BasicHashSet<KeyT, Derived, HashT, EqualT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		if (index == Npos)
		{
			return std::unexpected(Error {ErrorType::NotFound});
		}

		return KeyFrom(AsDerived().GetSlots()[index]);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashSet<KeyT, Derived, HashT, EqualT>::Add(const KeyT &key)
	{
		return AddInternal(key);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashSet<KeyT, Derived, HashT, EqualT>::Add(KeyT &&key)
	{
		return AddInternal(std::move(key));
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	bool BasicHashSet<KeyT, Derived, HashT, EqualT>::Remove(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index == Npos)
		{
			return false;
		}

		Slot &slot = AsDerived().GetSlots()[index];
		DestroySlot(slot);
		slot.status = SlotStatus::Deleted;
		--m_Size;
		++m_Deleted;

		return true;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	constexpr typename BasicHashSet<KeyT, Derived, HashT, EqualT>::SizeType BasicHashSet<KeyT, Derived, HashT, EqualT>::CalculateSlotCount(SizeType capacity, bool useMinimum)
	{
		SizeType required = capacity == 0 ? 1 : (capacity * LoadDenominator + LoadNumerator - 1) / LoadNumerator;
		SizeType slotCount = 1;

		while (slotCount < required)
		{
			slotCount *= 2;
		}

		if (useMinimum and capacity > 0 and slotCount < MinimumDynamicSlots)
		{
			slotCount = MinimumDynamicSlots;
		}

		return slotCount;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	constexpr typename BasicHashSet<KeyT, Derived, HashT, EqualT>::SizeType BasicHashSet<KeyT, Derived, HashT, EqualT>::CalculateCapacity(SizeType slotCount)
	{
		return slotCount * LoadNumerator / LoadDenominator;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	void BasicHashSet<KeyT, Derived, HashT, EqualT>::ClearSlots(Slot *slots, SizeType slotCount)
	{
		for (SizeType i = 0; i < slotCount; ++i)
		{
			if (slots[i].status == SlotStatus::Occupied)
			{
				DestroySlot(slots[i]);
			}

			slots[i].status = SlotStatus::Empty;
		}
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	void BasicHashSet<KeyT, Derived, HashT, EqualT>::MoveSlots(Slot *destination, SizeType destinationCount)
	{
		Slot *slots = AsDerived().GetSlots();
		SizeType slotCount = AsDerived().GetSlotCount();

		for (SizeType i = 0; i < slotCount; ++i)
		{
			if (slots[i].status != SlotStatus::Occupied)
			{
				continue;
			}

			SizeType target = FindAvailableIndexIn(destination, destinationCount, *KeyFrom(slots[i]));
			ConstructSlot(destination[target], std::move(*KeyFrom(slots[i])));
			DestroySlot(slots[i]);
			slots[i].status = SlotStatus::Empty;
		}

		m_Deleted = 0;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	void BasicHashSet<KeyT, Derived, HashT, EqualT>::ResetSize()
	{
		m_Size = 0;
		m_Deleted = 0;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	HashT BasicHashSet<KeyT, Derived, HashT, EqualT>::Hash() const
	{
		return m_Hash;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	EqualT BasicHashSet<KeyT, Derived, HashT, EqualT>::Equal() const
	{
		return m_Equal;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	Derived &BasicHashSet<KeyT, Derived, HashT, EqualT>::AsDerived()
	{
		return static_cast<Derived &>(*this);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	const Derived &BasicHashSet<KeyT, Derived, HashT, EqualT>::AsDerived() const
	{
		return static_cast<const Derived &>(*this);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashSet<KeyT, Derived, HashT, EqualT>::SizeType BasicHashSet<KeyT, Derived, HashT, EqualT>::FindIndex(const KeyT &key) const
	{
		const Slot *slots = AsDerived().GetSlots();
		SizeType slotCount = AsDerived().GetSlotCount();

		if (slots == nullptr or slotCount == 0)
		{
			return Npos;
		}

		SizeType index = m_Hash(key) & (slotCount - 1);

		for (SizeType i = 0; i < slotCount; ++i)
		{
			const Slot &slot = slots[(index + i) & (slotCount - 1)];

			if (slot.status == SlotStatus::Empty)
			{
				return Npos;
			}

			if (slot.status == SlotStatus::Occupied and m_Equal(*KeyFrom(slot), key))
			{
				return (index + i) & (slotCount - 1);
			}
		}

		return Npos;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashSet<KeyT, Derived, HashT, EqualT>::SizeType BasicHashSet<KeyT, Derived, HashT, EqualT>::FindAvailableIndex(const KeyT &key)
	{
		return FindAvailableIndexIn(AsDerived().GetSlots(), AsDerived().GetSlotCount(), key);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashSet<KeyT, Derived, HashT, EqualT>::SizeType BasicHashSet<KeyT, Derived, HashT, EqualT>::FindAvailableIndexIn(Slot *slots, SizeType slotCount, const KeyT &key)
	{
		if (slots == nullptr or slotCount == 0)
		{
			return Npos;
		}

		SizeType firstDeleted = Npos;
		SizeType index = m_Hash(key) & (slotCount - 1);

		for (SizeType i = 0; i < slotCount; ++i)
		{
			SizeType mapped = (index + i) & (slotCount - 1);
			Slot &slot = slots[mapped];

			if (slot.status == SlotStatus::Deleted and firstDeleted == Npos)
			{
				firstDeleted = mapped;
			}

			if (slot.status == SlotStatus::Empty)
			{
				return firstDeleted == Npos ? mapped : firstDeleted;
			}
		}

		return firstDeleted;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	KeyT *BasicHashSet<KeyT, Derived, HashT, EqualT>::KeyFrom(Slot &slot)
	{
		return reinterpret_cast<KeyT *>(slot.keyStorage);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	const KeyT *BasicHashSet<KeyT, Derived, HashT, EqualT>::KeyFrom(const Slot &slot)
	{
		return reinterpret_cast<const KeyT *>(slot.keyStorage);
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	template <typename KeyArg>
	std::expected<void, Error> BasicHashSet<KeyT, Derived, HashT, EqualT>::AddInternal(KeyArg &&key)
	{
		if (FindIndex(key) != Npos)
		{
			return std::unexpected(Error {ErrorType::AlreadyExists});
		}

		auto result = AsDerived().EnsureCapacity(m_Size + 1);

		if (not result)
		{
			return std::unexpected(result.error());
		}

		SizeType index = FindAvailableIndex(key);

		if (index == Npos)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
		}

		Slot &slot = AsDerived().GetSlots()[index];
		bool usedDeleted = slot.status == SlotStatus::Deleted;
		ConstructSlot(slot, std::forward<KeyArg>(key));
		++m_Size;

		if (usedDeleted)
		{
			--m_Deleted;
		}

		return {};
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	template <typename KeyArg>
	void BasicHashSet<KeyT, Derived, HashT, EqualT>::ConstructSlot(Slot &slot, KeyArg &&key)
	{
		new (slot.keyStorage) KeyT(std::forward<KeyArg>(key));
		slot.status = SlotStatus::Occupied;
	}

	template <typename KeyT, typename Derived, typename HashT, typename EqualT>
	void BasicHashSet<KeyT, Derived, HashT, EqualT>::DestroySlot(Slot &slot)
	{
		KeyFrom(slot)->~KeyT();
	}
} // namespace mlsl

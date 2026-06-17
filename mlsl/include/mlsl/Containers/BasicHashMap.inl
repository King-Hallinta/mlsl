// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::BasicHashMap(HashT hash, EqualT equal)
		: m_Hash(std::move(hash)), m_Equal(std::move(equal))
	{
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::SizeType BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Size() const
	{
		return m_Size;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::SizeType BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Capacity() const
	{
		return AsDerived().GetCapacity();
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	bool BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Empty() const
	{
		return m_Size == 0;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	void BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Clear()
	{
		ClearSlots(AsDerived().GetSlots(), AsDerived().GetSlotCount());
		m_Size = 0;
		m_Deleted = 0;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Reserve(SizeType capacity)
	{
		return AsDerived().EnsureCapacity(capacity);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	bool BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Contains(const KeyT &key) const
	{
		return FindIndex(key) != Npos;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	std::expected<typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Pointer, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Get(const KeyT &key)
	{
		SizeType index = FindIndex(key);

		if (index == Npos)
		{
			return std::unexpected(Error {ErrorType::NotFound});
		}

		return ValueFrom(AsDerived().GetSlots()[index]);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	std::expected<typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::ConstPointer, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Get(const KeyT &key) const
	{
		SizeType index = FindIndex(key);

		if (index == Npos)
		{
			return std::unexpected(Error {ErrorType::NotFound});
		}

		return ValueFrom(AsDerived().GetSlots()[index]);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Add(const KeyT &key, const ValueT &value)
	{
		return AddInternal(key, value);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Add(const KeyT &key, ValueT &&value)
	{
		return AddInternal(key, std::move(value));
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Add(KeyT &&key, const ValueT &value)
	{
		return AddInternal(std::move(key), value);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	std::expected<void, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Add(KeyT &&key, ValueT &&value)
	{
		return AddInternal(std::move(key), std::move(value));
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	bool BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Remove(const KeyT &key)
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

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	constexpr typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::SizeType BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::CalculateSlotCount(SizeType capacity, bool useMinimum)
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

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	constexpr typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::SizeType BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::CalculateCapacity(SizeType slotCount)
	{
		return slotCount * LoadNumerator / LoadDenominator;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	void BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::ClearSlots(Slot *slots, SizeType slotCount)
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

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	void BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::MoveSlots(Slot *destination, SizeType destinationCount)
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
			ConstructSlot(destination[target], std::move(*KeyFrom(slots[i])), std::move(*ValueFrom(slots[i])));
			DestroySlot(slots[i]);
			slots[i].status = SlotStatus::Empty;
		}

		m_Deleted = 0;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	void BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::ResetSize()
	{
		m_Size = 0;
		m_Deleted = 0;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	HashT BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Hash() const
	{
		return m_Hash;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	EqualT BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::Equal() const
	{
		return m_Equal;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	Derived &BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::AsDerived()
	{
		return static_cast<Derived &>(*this);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	const Derived &BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::AsDerived() const
	{
		return static_cast<const Derived &>(*this);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::SizeType BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::FindIndex(const KeyT &key) const
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

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::SizeType BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::FindAvailableIndex(const KeyT &key)
	{
		return FindAvailableIndexIn(AsDerived().GetSlots(), AsDerived().GetSlotCount(), key);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	typename BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::SizeType BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::FindAvailableIndexIn(Slot *slots, SizeType slotCount, const KeyT &key)
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

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	KeyT *BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::KeyFrom(Slot &slot)
	{
		return reinterpret_cast<KeyT *>(slot.keyStorage);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	const KeyT *BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::KeyFrom(const Slot &slot)
	{
		return reinterpret_cast<const KeyT *>(slot.keyStorage);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	ValueT *BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::ValueFrom(Slot &slot)
	{
		return reinterpret_cast<ValueT *>(slot.valueStorage);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	const ValueT *BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::ValueFrom(const Slot &slot)
	{
		return reinterpret_cast<const ValueT *>(slot.valueStorage);
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	template <typename KeyArg, typename ValueArg>
	std::expected<void, Error> BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::AddInternal(KeyArg &&key, ValueArg &&value)
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
		ConstructSlot(slot, std::forward<KeyArg>(key), std::forward<ValueArg>(value));
		++m_Size;

		if (usedDeleted)
		{
			--m_Deleted;
		}

		return {};
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	template <typename KeyArg, typename ValueArg>
	void BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::ConstructSlot(Slot &slot, KeyArg &&key, ValueArg &&value)
	{
		new (slot.keyStorage) KeyT(std::forward<KeyArg>(key));
		new (slot.valueStorage) ValueT(std::forward<ValueArg>(value));
		slot.status = SlotStatus::Occupied;
	}

	template <typename KeyT, typename ValueT, typename Derived, typename HashT, typename EqualT>
	void BasicHashMap<KeyT, ValueT, Derived, HashT, EqualT>::DestroySlot(Slot &slot)
	{
		ValueFrom(slot)->~ValueT();
		KeyFrom(slot)->~KeyT();
	}
} // namespace mlsl

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>
#include <functional>
#include <new>
#include <utility>

#include <mlsl/Core/Error.hpp>

namespace mlsl
{
	template <typename KeyT, typename Derived, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class BasicHashSet
	{
	public:
		using KeyType = KeyT;
		using ValueType = KeyT;
		using SizeType = std::size_t;
		using HashType = HashT;
		using EqualType = EqualT;
		using Pointer = KeyT *;
		using ConstPointer = const KeyT *;

	public:
		BasicHashSet() = default;
		BasicHashSet(HashT hash, EqualT equal);

	public:
		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] bool Empty() const;

		void Clear();
		[[nodiscard]] std::expected<void, Error> Reserve(SizeType capacity);

		[[nodiscard]] bool Contains(const KeyT &key) const;
		[[nodiscard]] std::expected<Pointer, Error> Get(const KeyT &key);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(const KeyT &key) const;

		std::expected<void, Error> Add(const KeyT &key);
		std::expected<void, Error> Add(KeyT &&key);
		[[nodiscard]] bool Remove(const KeyT &key);

	protected:
		enum class SlotStatus
		{
			Empty,
			Occupied,
			Deleted
		};

		struct Slot
		{
			SlotStatus status = SlotStatus::Empty;
			alignas(KeyT) std::byte keyStorage[sizeof(KeyT)];
		};

	protected:
		[[nodiscard]] static constexpr SizeType CalculateSlotCount(SizeType capacity, bool useMinimum);
		[[nodiscard]] static constexpr SizeType CalculateCapacity(SizeType slotCount);

		void ClearSlots(Slot *slots, SizeType slotCount);
		void MoveSlots(Slot *destination, SizeType destinationCount);
		void ResetSize();

		[[nodiscard]] static KeyT *KeyFrom(Slot &slot);
		[[nodiscard]] static const KeyT *KeyFrom(const Slot &slot);

		[[nodiscard]] HashT Hash() const;
		[[nodiscard]] EqualT Equal() const;

	protected:
		HashT m_Hash {};
		EqualT m_Equal {};
		SizeType m_Size = 0;
		SizeType m_Deleted = 0;

	private:
		static constexpr SizeType Npos = static_cast<SizeType>(-1);
		static constexpr SizeType MinimumDynamicSlots = 8;
		static constexpr SizeType LoadNumerator = 70;
		static constexpr SizeType LoadDenominator = 100;

	private:
		[[nodiscard]] Derived &AsDerived();
		[[nodiscard]] const Derived &AsDerived() const;

		[[nodiscard]] SizeType FindIndex(const KeyT &key) const;
		[[nodiscard]] SizeType FindAvailableIndex(const KeyT &key);
		[[nodiscard]] SizeType FindAvailableIndexIn(Slot *slots, SizeType slotCount, const KeyT &key);

		template <typename KeyArg>
		std::expected<void, Error> AddInternal(KeyArg &&key);

		template <typename KeyArg>
		void ConstructSlot(Slot &slot, KeyArg &&key);

		void DestroySlot(Slot &slot);

	private:
		friend Derived;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicHashSet.inl>

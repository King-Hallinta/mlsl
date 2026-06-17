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
	template <typename KeyT, typename ValueT, typename Derived, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class BasicHashMap
	{
	public:
		using KeyType = KeyT;
		using ValueType = ValueT;
		using SizeType = std::size_t;
		using HashType = HashT;
		using EqualType = EqualT;
		using Reference = ValueT &;
		using ConstReference = const ValueT &;
		using Pointer = ValueT *;
		using ConstPointer = const ValueT *;

	public:
		BasicHashMap() = default;
		BasicHashMap(HashT hash, EqualT equal);

	public:
		[[nodiscard]] SizeType Size() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] bool Empty() const;

		void Clear();
		[[nodiscard]] std::expected<void, Error> Reserve(SizeType capacity);

		[[nodiscard]] bool Contains(const KeyT &key) const;
		[[nodiscard]] std::expected<Pointer, Error> Get(const KeyT &key);
		[[nodiscard]] std::expected<ConstPointer, Error> Get(const KeyT &key) const;

		std::expected<void, Error> Add(const KeyT &key, const ValueT &value);
		std::expected<void, Error> Add(const KeyT &key, ValueT &&value);
		std::expected<void, Error> Add(KeyT &&key, const ValueT &value);
		std::expected<void, Error> Add(KeyT &&key, ValueT &&value);
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
			alignas(ValueT) std::byte valueStorage[sizeof(ValueT)];
		};

	protected:
		[[nodiscard]] static constexpr SizeType CalculateSlotCount(SizeType capacity, bool useMinimum);
		[[nodiscard]] static constexpr SizeType CalculateCapacity(SizeType slotCount);

		void ClearSlots(Slot *slots, SizeType slotCount);
		void MoveSlots(Slot *destination, SizeType destinationCount);
		void ResetSize();

		[[nodiscard]] static KeyT *KeyFrom(Slot &slot);
		[[nodiscard]] static const KeyT *KeyFrom(const Slot &slot);
		[[nodiscard]] static ValueT *ValueFrom(Slot &slot);
		[[nodiscard]] static const ValueT *ValueFrom(const Slot &slot);

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

		template <typename KeyArg, typename ValueArg>
		std::expected<void, Error> AddInternal(KeyArg &&key, ValueArg &&value);

		template <typename KeyArg, typename ValueArg>
		void ConstructSlot(Slot &slot, KeyArg &&key, ValueArg &&value);

		void DestroySlot(Slot &slot);

	private:
		friend Derived;
	};
} // namespace mlsl

#include <mlsl/Containers/BasicHashMap.inl>

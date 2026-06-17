// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicHashMap.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class FixedHashMap : public BasicHashMap<KeyT, ValueT, FixedHashMap<KeyT, ValueT, N, HashT, EqualT>, HashT, EqualT>
	{
	public:
		using Base = BasicHashMap<KeyT, ValueT, FixedHashMap<KeyT, ValueT, N, HashT, EqualT>, HashT, EqualT>;
		using typename Base::SizeType;

	private:
		using Slot = typename Base::Slot;
		static constexpr SizeType SlotCount = Base::CalculateSlotCount(N, false);

	public:
		FixedHashMap() = default;
		FixedHashMap(HashT hash, EqualT equal = EqualT {});
		FixedHashMap(const FixedHashMap &other);
		FixedHashMap(FixedHashMap &&other) noexcept;
		~FixedHashMap();

	public:
		FixedHashMap &operator=(const FixedHashMap &other);
		FixedHashMap &operator=(FixedHashMap &&other) noexcept;

	private:
		[[nodiscard]] Slot *GetSlots();
		[[nodiscard]] const Slot *GetSlots() const;
		[[nodiscard]] SizeType GetSlotCount() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		void CopyFrom(const FixedHashMap &other);
		void MoveFrom(FixedHashMap &other);

	private:
		Slot m_Slots[SlotCount];

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/FixedHashMap.inl>

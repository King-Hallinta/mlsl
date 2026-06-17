// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicHashSet.hpp>

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class FixedHashSet : public BasicHashSet<KeyT, FixedHashSet<KeyT, N, HashT, EqualT>, HashT, EqualT>
	{
	public:
		using Base = BasicHashSet<KeyT, FixedHashSet<KeyT, N, HashT, EqualT>, HashT, EqualT>;
		using typename Base::SizeType;

	private:
		using Slot = typename Base::Slot;
		static constexpr SizeType SlotCount = Base::CalculateSlotCount(N, false);

	public:
		FixedHashSet() = default;
		FixedHashSet(HashT hash, EqualT equal = EqualT {});
		FixedHashSet(const FixedHashSet &other);
		FixedHashSet(FixedHashSet &&other) noexcept;
		~FixedHashSet();

	public:
		FixedHashSet &operator=(const FixedHashSet &other);
		FixedHashSet &operator=(FixedHashSet &&other) noexcept;

	private:
		[[nodiscard]] Slot *GetSlots();
		[[nodiscard]] const Slot *GetSlots() const;
		[[nodiscard]] SizeType GetSlotCount() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		void CopyFrom(const FixedHashSet &other);
		void MoveFrom(FixedHashSet &other);

	private:
		Slot m_Slots[SlotCount];

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/FixedHashSet.inl>

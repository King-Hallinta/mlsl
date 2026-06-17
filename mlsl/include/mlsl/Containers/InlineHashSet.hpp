// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicHashSet.hpp>

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class InlineHashSet : public BasicHashSet<KeyT, InlineHashSet<KeyT, N, HashT, EqualT>, HashT, EqualT>
	{
	public:
		using Base = BasicHashSet<KeyT, InlineHashSet<KeyT, N, HashT, EqualT>, HashT, EqualT>;
		using typename Base::SizeType;

	private:
		using Slot = typename Base::Slot;
		static constexpr SizeType InlineSlotCount = Base::CalculateSlotCount(N, false);

	public:
		InlineHashSet();
		explicit InlineHashSet(Allocator *allocator);
		InlineHashSet(HashT hash, EqualT equal = EqualT {});
		InlineHashSet(Allocator *allocator, HashT hash, EqualT equal = EqualT {});
		InlineHashSet(const InlineHashSet &other);
		InlineHashSet(InlineHashSet &&other) noexcept;
		~InlineHashSet();

	public:
		InlineHashSet &operator=(const InlineHashSet &other);
		InlineHashSet &operator=(InlineHashSet &&other) noexcept;

	public:
		[[nodiscard]] SizeType InlineCapacity() const;

	private:
		[[nodiscard]] Slot *GetSlots();
		[[nodiscard]] const Slot *GetSlots() const;
		[[nodiscard]] SizeType GetSlotCount() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		[[nodiscard]] bool IsInline() const;
		[[nodiscard]] std::expected<Slot *, Error> AllocateSlots(SizeType slotCount);
		void DestroySlotArray(Slot *slots, SizeType slotCount);
		void ReleaseHeapSlots();
		std::expected<void, Error> Rebuild(SizeType slotCount);
		void CopyFrom(const InlineHashSet &other);
		void MoveFrom(InlineHashSet &other);

	private:
		Slot m_InlineSlots[InlineSlotCount];
		Slot *m_Slots = m_InlineSlots;
		SizeType m_SlotCount = InlineSlotCount;
		SizeType m_Capacity = N;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/InlineHashSet.inl>

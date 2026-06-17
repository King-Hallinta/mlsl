// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicHashMap.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class InlineHashMap : public BasicHashMap<KeyT, ValueT, InlineHashMap<KeyT, ValueT, N, HashT, EqualT>, HashT, EqualT>
	{
	public:
		using Base = BasicHashMap<KeyT, ValueT, InlineHashMap<KeyT, ValueT, N, HashT, EqualT>, HashT, EqualT>;
		using typename Base::SizeType;

	private:
		using Slot = typename Base::Slot;
		static constexpr SizeType InlineSlotCount = Base::CalculateSlotCount(N, false);

	public:
		InlineHashMap();
		explicit InlineHashMap(Allocator *allocator);
		InlineHashMap(HashT hash, EqualT equal = EqualT {});
		InlineHashMap(Allocator *allocator, HashT hash, EqualT equal = EqualT {});
		InlineHashMap(const InlineHashMap &other);
		InlineHashMap(InlineHashMap &&other) noexcept;
		~InlineHashMap();

	public:
		InlineHashMap &operator=(const InlineHashMap &other);
		InlineHashMap &operator=(InlineHashMap &&other) noexcept;

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
		void CopyFrom(const InlineHashMap &other);
		void MoveFrom(InlineHashMap &other);

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

#include <mlsl/Containers/InlineHashMap.inl>

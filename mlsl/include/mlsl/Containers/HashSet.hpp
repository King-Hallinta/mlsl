// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicHashSet.hpp>

namespace mlsl
{
	template <typename KeyT, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class HashSet : public BasicHashSet<KeyT, HashSet<KeyT, HashT, EqualT>, HashT, EqualT>
	{
	public:
		using Base = BasicHashSet<KeyT, HashSet<KeyT, HashT, EqualT>, HashT, EqualT>;
		using typename Base::SizeType;

	private:
		using Slot = typename Base::Slot;

	public:
		HashSet();
		explicit HashSet(Allocator *allocator);
		HashSet(HashT hash, EqualT equal = EqualT {});
		HashSet(Allocator *allocator, HashT hash, EqualT equal = EqualT {});
		HashSet(const HashSet &other);
		HashSet(HashSet &&other) noexcept;
		~HashSet();

	public:
		HashSet &operator=(const HashSet &other);
		HashSet &operator=(HashSet &&other) noexcept;

	private:
		[[nodiscard]] Slot *GetSlots();
		[[nodiscard]] const Slot *GetSlots() const;
		[[nodiscard]] SizeType GetSlotCount() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		[[nodiscard]] std::expected<Slot *, Error> AllocateSlots(SizeType slotCount);
		void DestroySlotArray(Slot *slots, SizeType slotCount);
		void ReleaseSlots();
		std::expected<void, Error> Rebuild(SizeType slotCount);
		void CopyFrom(const HashSet &other);

	private:
		Slot *m_Slots = nullptr;
		SizeType m_SlotCount = 0;
		SizeType m_Capacity = 0;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/HashSet.inl>

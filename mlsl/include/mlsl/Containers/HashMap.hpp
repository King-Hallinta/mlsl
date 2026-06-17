// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicHashMap.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, typename HashT = std::hash<KeyT>, typename EqualT = std::equal_to<KeyT>>
	class HashMap : public BasicHashMap<KeyT, ValueT, HashMap<KeyT, ValueT, HashT, EqualT>, HashT, EqualT>
	{
	public:
		using Base = BasicHashMap<KeyT, ValueT, HashMap<KeyT, ValueT, HashT, EqualT>, HashT, EqualT>;
		using typename Base::SizeType;

	private:
		using Slot = typename Base::Slot;

	public:
		HashMap();
		explicit HashMap(Allocator *allocator);
		HashMap(HashT hash, EqualT equal = EqualT {});
		HashMap(Allocator *allocator, HashT hash, EqualT equal = EqualT {});
		HashMap(const HashMap &other);
		HashMap(HashMap &&other) noexcept;
		~HashMap();

	public:
		HashMap &operator=(const HashMap &other);
		HashMap &operator=(HashMap &&other) noexcept;

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
		void CopyFrom(const HashMap &other);

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

#include <mlsl/Containers/HashMap.inl>

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicSet.hpp>

namespace mlsl
{
	template <typename KeyT, typename LessT = std::less<KeyT>>
	class Set : public BasicSet<KeyT, Set<KeyT, LessT>, LessT>
	{
	public:
		using Base = BasicSet<KeyT, Set<KeyT, LessT>, LessT>;
		using typename Base::SizeType;

	public:
		Set();
		explicit Set(Allocator *allocator);
		Set(LessT less);
		Set(Allocator *allocator, LessT less);
		Set(const Set &other);
		Set(Set &&other) noexcept;
		~Set();

	public:
		Set &operator=(const Set &other);
		Set &operator=(Set &&other) noexcept;

	private:
		[[nodiscard]] KeyT *GetKeys();
		[[nodiscard]] const KeyT *GetKeys() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		[[nodiscard]] std::expected<KeyT *, Error> AllocateKeys(SizeType capacity);
		std::expected<void, Error> Grow(SizeType capacity);
		void ReleaseHeapKeys();
		void CopyFrom(const Set &other);
		void StealFrom(Set &other);

	private:
		KeyT *m_Keys = nullptr;
		SizeType m_Capacity = 0;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/Set.inl>

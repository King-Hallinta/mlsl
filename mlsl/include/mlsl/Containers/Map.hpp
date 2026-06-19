// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicMap.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, typename LessT = std::less<KeyT>>
	class Map : public BasicMap<KeyT, ValueT, Map<KeyT, ValueT, LessT>, LessT>
	{
	public:
		using Base = BasicMap<KeyT, ValueT, Map<KeyT, ValueT, LessT>, LessT>;
		using typename Base::SizeType;

	private:
		using Entry = typename Base::Entry;

	public:
		Map();
		explicit Map(Allocator *allocator);
		Map(LessT less);
		Map(Allocator *allocator, LessT less);
		Map(const Map &other);
		Map(Map &&other) noexcept;
		~Map();

	public:
		Map &operator=(const Map &other);
		Map &operator=(Map &&other) noexcept;

	private:
		[[nodiscard]] Entry *GetEntries();
		[[nodiscard]] const Entry *GetEntries() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		[[nodiscard]] std::expected<Entry *, Error> AllocateEntries(SizeType capacity);
		std::expected<void, Error> Grow(SizeType capacity);
		void ReleaseHeapEntries();
		void CopyFrom(const Map &other);
		void StealFrom(Map &other);

	private:
		Entry *m_Entries = nullptr;
		SizeType m_Capacity = 0;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/Map.inl>

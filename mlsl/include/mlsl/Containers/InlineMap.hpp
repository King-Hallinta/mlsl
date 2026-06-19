// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicMap.hpp>

namespace mlsl
{
	template <typename KeyT, typename ValueT, std::size_t N, typename LessT = std::less<KeyT>>
	class InlineMap : public BasicMap<KeyT, ValueT, InlineMap<KeyT, ValueT, N, LessT>, LessT>
	{
	public:
		using Base = BasicMap<KeyT, ValueT, InlineMap<KeyT, ValueT, N, LessT>, LessT>;
		using typename Base::SizeType;

	private:
		using Entry = typename Base::Entry;
		static constexpr SizeType InlineStorageCount = N == 0 ? 1 : N;

	public:
		InlineMap();
		explicit InlineMap(Allocator *allocator);
		InlineMap(LessT less);
		InlineMap(Allocator *allocator, LessT less);
		InlineMap(const InlineMap &other);
		InlineMap(InlineMap &&other) noexcept;
		~InlineMap();

	public:
		InlineMap &operator=(const InlineMap &other);
		InlineMap &operator=(InlineMap &&other) noexcept;

	public:
		[[nodiscard]] SizeType InlineCapacity() const;

	private:
		[[nodiscard]] Entry *GetEntries();
		[[nodiscard]] const Entry *GetEntries() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		[[nodiscard]] Entry *InlineEntries();
		[[nodiscard]] const Entry *InlineEntries() const;
		[[nodiscard]] bool IsInline() const;
		[[nodiscard]] std::expected<Entry *, Error> AllocateEntries(SizeType capacity);
		std::expected<void, Error> Grow(SizeType capacity);
		void ReleaseHeapEntries();
		void CopyFrom(const InlineMap &other);
		void MoveFrom(InlineMap &other);

	private:
		alignas(Entry) std::byte m_InlineStorage[sizeof(Entry) * InlineStorageCount];
		Entry *m_Entries = InlineEntries();
		SizeType m_Capacity = N;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/InlineMap.inl>

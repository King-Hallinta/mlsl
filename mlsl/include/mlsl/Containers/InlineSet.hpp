// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicSet.hpp>

namespace mlsl
{
	template <typename KeyT, std::size_t N, typename LessT = std::less<KeyT>>
	class InlineSet : public BasicSet<KeyT, InlineSet<KeyT, N, LessT>, LessT>
	{
	public:
		using Base = BasicSet<KeyT, InlineSet<KeyT, N, LessT>, LessT>;
		using typename Base::SizeType;

	private:
		static constexpr SizeType InlineStorageCount = N == 0 ? 1 : N;

	public:
		InlineSet();
		explicit InlineSet(Allocator *allocator);
		InlineSet(LessT less);
		InlineSet(Allocator *allocator, LessT less);
		InlineSet(const InlineSet &other);
		InlineSet(InlineSet &&other) noexcept;
		~InlineSet();

	public:
		InlineSet &operator=(const InlineSet &other);
		InlineSet &operator=(InlineSet &&other) noexcept;

	public:
		[[nodiscard]] SizeType InlineCapacity() const;

	private:
		[[nodiscard]] KeyT *GetKeys();
		[[nodiscard]] const KeyT *GetKeys() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType capacity);

		[[nodiscard]] KeyT *InlineKeys();
		[[nodiscard]] const KeyT *InlineKeys() const;
		[[nodiscard]] bool IsInline() const;
		[[nodiscard]] std::expected<KeyT *, Error> AllocateKeys(SizeType capacity);
		std::expected<void, Error> Grow(SizeType capacity);
		void ReleaseHeapKeys();
		void CopyFrom(const InlineSet &other);
		void MoveFrom(InlineSet &other);

	private:
		alignas(KeyT) std::byte m_InlineStorage[sizeof(KeyT) * InlineStorageCount];
		KeyT *m_Keys = InlineKeys();
		SizeType m_Capacity = N;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/InlineSet.inl>

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicList.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class InlineList : public BasicList<T, InlineList<T, N>>
	{
	public:
		using Base = BasicList<T, InlineList<T, N>>;
		using typename Base::NodeType;
		using typename Base::SizeType;

	public:
		InlineList();
		explicit InlineList(Allocator *allocator);
		InlineList(const InlineList &other);
		InlineList(InlineList &&other) noexcept;
		~InlineList();

	public:
		InlineList &operator=(const InlineList &other);
		InlineList &operator=(InlineList &&other) noexcept;

	public:
		[[nodiscard]] std::expected<NodeType *, Error> AllocateNode();
		void DeallocateNode(NodeType *node);

		[[nodiscard]] SizeType InlineCapacity() const;

	private:
		bool IsInline(NodeType *node) const;

	private:
		union Slot {
			alignas(NodeType) std::byte storage[sizeof(NodeType)];
			Slot *next;
		};

	private:
		Slot m_Storage[N];
		Slot *m_FreeList = nullptr;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/InlineList.inl>

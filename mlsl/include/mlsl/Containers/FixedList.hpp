// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicList.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class FixedList : public BasicList<T, FixedList<T, N>>
	{
	public:
		using Base = BasicList<T, FixedList<T, N>>;
		using typename Base::NodeType;
		using typename Base::SizeType;

	public:
		FixedList();
		FixedList(const FixedList &other);
		FixedList(FixedList &&other) noexcept;
		~FixedList();

	public:
		FixedList &operator=(const FixedList &other);
		FixedList &operator=(FixedList &&other) noexcept;

	public:
		[[nodiscard]] std::expected<NodeType *, Error> AllocateNode();
		void DeallocateNode(NodeType *node);

		[[nodiscard]] SizeType Capacity() const;

	private:
		union Slot {
			alignas(NodeType) std::byte storage[sizeof(NodeType)];
			Slot *next;
		};

	private:
		Slot m_Storage[N];
		Slot *m_FreeList = nullptr;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/FixedList.inl>

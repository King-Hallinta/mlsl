// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicList.hpp>

namespace mlsl
{
	template <typename T>
	class List : public BasicList<T, List<T>>
	{
	public:
		using Base = BasicList<T, List<T>>;
		using typename Base::NodeType;
		using typename Base::SizeType;

	public:
		List();
		explicit List(Allocator *allocator);
		List(const List &other);
		List(List &&other) noexcept;
		~List();

	public:
		List &operator=(const List &other);
		List &operator=(List &&other) noexcept;

	public:
		[[nodiscard]] std::expected<NodeType *, Error> AllocateNode();
		void DeallocateNode(NodeType *node);

	private:
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/List.inl>

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicQueue.hpp>

namespace mlsl
{
	template <typename T>
	class Queue : public BasicQueue<T, Queue<T>>
	{
	public:
		using Base = BasicQueue<T, Queue<T>>;
		using typename Base::SizeType;

	public:
		Queue();
		explicit Queue(Allocator *allocator);
		Queue(const Queue &other);
		Queue(Queue &&other) noexcept;
		~Queue();

	public:
		Queue &operator=(const Queue &other);
		Queue &operator=(Queue &&other) noexcept;

	public:
		[[nodiscard]] T *GetStorage();
		[[nodiscard]] const T *GetStorage() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType required);

		[[nodiscard]] SizeType Capacity() const;
		std::expected<void, Error> Reserve(SizeType capacity);

	private:
		std::expected<void, Error> Grow(SizeType newCapacity);

	private:
		T *m_Storage = nullptr;
		SizeType m_Capacity = 0;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/Queue.inl>

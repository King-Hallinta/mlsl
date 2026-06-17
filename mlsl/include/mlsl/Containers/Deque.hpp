// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicDeque.hpp>

namespace mlsl
{
	template <typename T>
	class Deque : public BasicDeque<T, Deque<T>>
	{
	public:
		using Base = BasicDeque<T, Deque<T>>;
		using typename Base::SizeType;

	public:
		Deque();
		explicit Deque(Allocator *allocator);
		Deque(const Deque &other);
		Deque(Deque &&other) noexcept;
		~Deque();

	public:
		Deque &operator=(const Deque &other);
		Deque &operator=(Deque &&other) noexcept;

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

#include <mlsl/Containers/Deque.inl>

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicRingBuffer.hpp>

namespace mlsl
{
	template <typename T>
	class RingBuffer : public BasicRingBuffer<T, RingBuffer<T>>
	{
	public:
		using Base = BasicRingBuffer<T, RingBuffer<T>>;
		using typename Base::SizeType;

	public:
		RingBuffer();
		explicit RingBuffer(Allocator *allocator);
		RingBuffer(const RingBuffer &other);
		RingBuffer(RingBuffer &&other) noexcept;
		~RingBuffer();

	public:
		RingBuffer &operator=(const RingBuffer &other);
		RingBuffer &operator=(RingBuffer &&other) noexcept;

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

#include <mlsl/Containers/RingBuffer.inl>

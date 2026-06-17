// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicRingBuffer.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class FixedRingBuffer : public BasicRingBuffer<T, FixedRingBuffer<T, N>>
	{
	public:
		using Base = BasicRingBuffer<T, FixedRingBuffer<T, N>>;
		using typename Base::SizeType;

	public:
		FixedRingBuffer() = default;
		FixedRingBuffer(const FixedRingBuffer &other);
		FixedRingBuffer(FixedRingBuffer &&other) noexcept;
		~FixedRingBuffer();

	public:
		FixedRingBuffer &operator=(const FixedRingBuffer &other);
		FixedRingBuffer &operator=(FixedRingBuffer &&other) noexcept;

	public:
		[[nodiscard]] T *GetStorage();
		[[nodiscard]] const T *GetStorage() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType required);

		[[nodiscard]] SizeType Capacity() const;

	private:
		alignas(T) std::byte m_Storage[sizeof(T) * N];

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/FixedRingBuffer.inl>

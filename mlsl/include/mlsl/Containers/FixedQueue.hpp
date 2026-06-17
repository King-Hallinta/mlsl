// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicQueue.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class FixedQueue : public BasicQueue<T, FixedQueue<T, N>>
	{
	public:
		using Base = BasicQueue<T, FixedQueue<T, N>>;
		using typename Base::SizeType;

	public:
		FixedQueue() = default;
		FixedQueue(const FixedQueue &other);
		FixedQueue(FixedQueue &&other) noexcept;
		~FixedQueue();

	public:
		FixedQueue &operator=(const FixedQueue &other);
		FixedQueue &operator=(FixedQueue &&other) noexcept;

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

#include <mlsl/Containers/FixedQueue.inl>

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicDeque.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class FixedDeque : public BasicDeque<T, FixedDeque<T, N>>
	{
	public:
		using Base = BasicDeque<T, FixedDeque<T, N>>;
		using typename Base::SizeType;

	public:
		FixedDeque() = default;
		FixedDeque(const FixedDeque &other);
		FixedDeque(FixedDeque &&other) noexcept;
		~FixedDeque();

	public:
		FixedDeque &operator=(const FixedDeque &other);
		FixedDeque &operator=(FixedDeque &&other) noexcept;

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

#include <mlsl/Containers/FixedDeque.inl>

// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Containers/BasicStack.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class FixedStack : public BasicStack<T, FixedStack<T, N>>
	{
	public:
		using Base = BasicStack<T, FixedStack<T, N>>;
		using typename Base::SizeType;

	public:
		FixedStack() = default;
		FixedStack(const FixedStack &other);
		FixedStack(FixedStack &&other) noexcept;
		~FixedStack();

	public:
		FixedStack &operator=(const FixedStack &other);
		FixedStack &operator=(FixedStack &&other) noexcept;

	public:
		[[nodiscard]] T *GetStorage();
		[[nodiscard]] const T *GetStorage() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType required);

		[[nodiscard]] SizeType Capacity() const;

	private:
		static constexpr SizeType StorageCount = N == 0 ? 1 : N;
		alignas(T) std::byte m_Storage[sizeof(T) * StorageCount];

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/FixedStack.inl>

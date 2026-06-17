// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>
#include <mlsl/Allocators/DefaultAllocator.hpp>
#include <mlsl/Containers/BasicDeque.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class InlineDeque : public BasicDeque<T, InlineDeque<T, N>>
	{
	public:
		using Base = BasicDeque<T, InlineDeque<T, N>>;
		using typename Base::SizeType;

	public:
		InlineDeque();
		explicit InlineDeque(Allocator *allocator);
		InlineDeque(const InlineDeque &other);
		InlineDeque(InlineDeque &&other) noexcept;
		~InlineDeque();

	public:
		InlineDeque &operator=(const InlineDeque &other);
		InlineDeque &operator=(InlineDeque &&other) noexcept;

	public:
		[[nodiscard]] T *GetStorage();
		[[nodiscard]] const T *GetStorage() const;
		[[nodiscard]] SizeType GetCapacity() const;
		[[nodiscard]] std::expected<void, Error> EnsureCapacity(SizeType required);

		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] SizeType InlineCapacity() const;

	private:
		bool IsInline() const;
		std::expected<void, Error> Grow(SizeType newCapacity);

	private:
		alignas(T) std::byte m_InlineStorage[sizeof(T) * N];
		T *m_Storage = nullptr;
		SizeType m_Capacity = N;
		Allocator *m_Allocator = nullptr;
		bool m_OwnsAllocator = false;

	private:
		friend Base;
	};
} // namespace mlsl

#include <mlsl/Containers/InlineDeque.inl>

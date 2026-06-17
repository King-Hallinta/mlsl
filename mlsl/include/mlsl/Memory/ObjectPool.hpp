// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <expected>

#include <mlsl/Core/Error.hpp>
#include <mlsl/Core/Macros.hpp>

namespace mlsl
{
	template <typename T, std::size_t N>
	class ObjectPool
	{
	public:
		using ValueType = T;
		using SizeType = std::size_t;
		using Pointer = T *;

	public:
		ObjectPool();
		ObjectPool(const ObjectPool &) = delete;
		ObjectPool(ObjectPool &&) = delete;
		~ObjectPool();

	public:
		ObjectPool &operator=(const ObjectPool &) = delete;
		ObjectPool &operator=(ObjectPool &&) = delete;

	public:
		template <typename... Args>
		[[nodiscard]] std::expected<Pointer, Error> Acquire(Args &&...args);
		void Release(Pointer ptr);

		[[nodiscard]] SizeType Available() const;
		[[nodiscard]] SizeType Capacity() const;
		[[nodiscard]] bool Owns(Pointer ptr) const;

	private:
		union Slot {
			alignas(T) std::byte storage[sizeof(T)];
			Slot *next;
		};

	private:
		Slot m_Storage[N];
		Slot *m_FreeList = nullptr;
		SizeType m_Available = N;
	};
} // namespace mlsl

#include <mlsl/Memory/ObjectPool.inl>

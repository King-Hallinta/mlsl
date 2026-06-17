// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <utility>

namespace mlsl
{
	template <typename T>
	class UniquePtr
	{
	public:
		using Pointer = T *;
		using Reference = T &;

	public:
		constexpr UniquePtr() = default;
		constexpr explicit UniquePtr(Pointer ptr);
		constexpr UniquePtr(std::nullptr_t);
		constexpr UniquePtr(const UniquePtr &) = delete;
		constexpr UniquePtr(UniquePtr &&other) noexcept;
		template <typename U>
		constexpr UniquePtr(UniquePtr<U> &&other) noexcept;
		constexpr ~UniquePtr();

	public:
		constexpr UniquePtr &operator=(const UniquePtr &) = delete;
		constexpr UniquePtr &operator=(UniquePtr &&other) noexcept;
		constexpr UniquePtr &operator=(std::nullptr_t) noexcept;
		template <typename U>
		constexpr UniquePtr &operator=(UniquePtr<U> &&other) noexcept;

	public:
		template <typename... Args>
		[[nodiscard]] static constexpr UniquePtr Create(Args &&...args);

	public:
		[[nodiscard]] constexpr Pointer Get() const;
		[[nodiscard]] constexpr Pointer operator->() const;
		[[nodiscard]] constexpr Reference operator*() const;
		[[nodiscard]] constexpr explicit operator bool() const;

		constexpr void Reset(Pointer ptr = nullptr);
		[[nodiscard]] constexpr Pointer Release();
		constexpr void Swap(UniquePtr &other) noexcept;

	private:
		Pointer m_Ptr = nullptr;

	private:
		template <typename U>
		friend class UniquePtr;
	};
} // namespace mlsl

#include <mlsl/Memory/UniquePtr.inl>

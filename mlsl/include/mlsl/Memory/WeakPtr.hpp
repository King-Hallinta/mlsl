// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>

#include <mlsl/Memory/SharedPtr.hpp>

namespace mlsl
{
	template <typename T>
	class WeakPtr
	{
	public:
		using Pointer = T *;

	public:
		WeakPtr() = default;
		WeakPtr(std::nullptr_t);
		WeakPtr(const SharedPtr<T> &shared);
		WeakPtr(const WeakPtr &other);
		WeakPtr(WeakPtr &&other) noexcept;
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		WeakPtr(const SharedPtr<U> &shared);
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		WeakPtr(const WeakPtr<U> &other);
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		WeakPtr(WeakPtr<U> &&other) noexcept;
		~WeakPtr();

	public:
		WeakPtr &operator=(const SharedPtr<T> &shared);
		WeakPtr &operator=(const WeakPtr &other);
		WeakPtr &operator=(WeakPtr &&other) noexcept;
		WeakPtr &operator=(std::nullptr_t) noexcept;
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		WeakPtr &operator=(const SharedPtr<U> &shared);
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		WeakPtr &operator=(const WeakPtr<U> &other);
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		WeakPtr &operator=(WeakPtr<U> &&other) noexcept;

	public:
		[[nodiscard]] std::optional<SharedPtr<T>> Lock() const;
		[[nodiscard]] bool Expired() const;
		[[nodiscard]] std::uint32_t RefCount() const;
		void Reset();
		void Swap(WeakPtr &other) noexcept;

	private:
		void Release();

		Pointer m_Ptr = nullptr;
		SharedControlBlock *m_Control = nullptr;

	private:
		template <typename U>
		friend class WeakPtr;
		template <typename U>
		friend class SharedPtr;
	};
} // namespace mlsl

#include <mlsl/Memory/WeakPtr.inl>

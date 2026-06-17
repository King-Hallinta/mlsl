// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>
#include <utility>

#include <mlsl/Memory/SharedControlBlock.hpp>

namespace mlsl
{
	template <typename T>
	class WeakPtr;

	template <typename T>
	class SharedPtr
	{
	public:
		using Pointer = T *;
		using Reference = T &;

	public:
		SharedPtr() = default;
		explicit SharedPtr(Pointer ptr);
		SharedPtr(std::nullptr_t);
		SharedPtr(const SharedPtr &other);
		SharedPtr(SharedPtr &&other) noexcept;
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		SharedPtr(const SharedPtr<U> &other);
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		SharedPtr(SharedPtr<U> &&other) noexcept;
		explicit SharedPtr(const WeakPtr<T> &weak);
		~SharedPtr();

	public:
		SharedPtr &operator=(const SharedPtr &other);
		SharedPtr &operator=(SharedPtr &&other) noexcept;
		SharedPtr &operator=(std::nullptr_t) noexcept;
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		SharedPtr &operator=(const SharedPtr<U> &other);
		template <typename U>
			requires std::is_convertible_v<U *, T *>
		SharedPtr &operator=(SharedPtr<U> &&other) noexcept;

	public:
		template <typename... Args>
		[[nodiscard]] static SharedPtr Create(Args &&...args);

	public:
		[[nodiscard]] Pointer Get() const;
		[[nodiscard]] Pointer operator->() const;
		[[nodiscard]] Reference operator*() const;
		[[nodiscard]] explicit operator bool() const;

		[[nodiscard]] std::uint32_t RefCount() const;
		void Reset(Pointer ptr = nullptr);
		void Swap(SharedPtr &other) noexcept;

	private:
		void Release();

		Pointer m_Ptr = nullptr;
		SharedControlBlock *m_Control = nullptr;

	private:
		template <typename U>
		friend class SharedPtr;
		template <typename U>
		friend class WeakPtr;
	};
} // namespace mlsl

#include <mlsl/Memory/SharedPtr.inl>

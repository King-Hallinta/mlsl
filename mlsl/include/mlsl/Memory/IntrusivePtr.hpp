// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstddef>
#include <type_traits>
#include <utility>

namespace mlsl
{
	template <typename T>
	struct IntrusivePointerTraits
	{
		static void AddReference(T *object);
		static void ReleaseReference(T *object);
	};

	template <typename T, typename Traits = IntrusivePointerTraits<T>>
	class IntrusivePtr
	{
	public:
		using Pointer = T *;
		using Reference = T &;
		using TraitsType = Traits;

	public:
		IntrusivePtr() = default;
		explicit IntrusivePtr(Pointer ptr);
		IntrusivePtr(std::nullptr_t);
		IntrusivePtr(const IntrusivePtr &other);
		IntrusivePtr(IntrusivePtr &&other) noexcept;
		template <typename U, typename OtherTraits>
			requires std::is_convertible_v<U *, T *>
		IntrusivePtr(const IntrusivePtr<U, OtherTraits> &other);
		template <typename U, typename OtherTraits>
			requires std::is_convertible_v<U *, T *>
		IntrusivePtr(IntrusivePtr<U, OtherTraits> &&other) noexcept;
		~IntrusivePtr();

	public:
		IntrusivePtr &operator=(const IntrusivePtr &other);
		IntrusivePtr &operator=(IntrusivePtr &&other) noexcept;
		IntrusivePtr &operator=(std::nullptr_t) noexcept;
		template <typename U, typename OtherTraits>
			requires std::is_convertible_v<U *, T *>
		IntrusivePtr &operator=(const IntrusivePtr<U, OtherTraits> &other);
		template <typename U, typename OtherTraits>
			requires std::is_convertible_v<U *, T *>
		IntrusivePtr &operator=(IntrusivePtr<U, OtherTraits> &&other) noexcept;

	public:
		[[nodiscard]] Pointer Get() const;
		[[nodiscard]] Pointer operator->() const;
		[[nodiscard]] Reference operator*() const;
		[[nodiscard]] explicit operator bool() const;

		void Reset(Pointer ptr = nullptr);
		[[nodiscard]] Pointer Release();
		void Swap(IntrusivePtr &other) noexcept;

	private:
		void AddReference();
		void ReleaseReference();

		Pointer m_Ptr = nullptr;

	private:
		template <typename U, typename OtherTraits>
		friend class IntrusivePtr;
	};
} // namespace mlsl

#include <mlsl/Memory/IntrusivePtr.inl>

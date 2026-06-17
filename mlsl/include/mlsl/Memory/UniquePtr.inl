// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <typename T>
	constexpr UniquePtr<T>::UniquePtr(Pointer ptr)
		: m_Ptr(ptr)
	{
	}

	template <typename T>
	constexpr UniquePtr<T>::UniquePtr(std::nullptr_t)
		: m_Ptr(nullptr)
	{
	}

	template <typename T>
	constexpr UniquePtr<T>::UniquePtr(UniquePtr &&other) noexcept
		: m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}

	template <typename T>
	template <typename U>
	constexpr UniquePtr<T>::UniquePtr(UniquePtr<U> &&other) noexcept
		: m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}

	template <typename T>
	constexpr UniquePtr<T>::~UniquePtr()
	{
		delete m_Ptr;
	}

	template <typename T>
	constexpr UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr &&other) noexcept
	{
		if (this != &other)
		{
			delete m_Ptr;
			m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
		}

		return *this;
	}

	template <typename T>
	constexpr UniquePtr<T> &UniquePtr<T>::operator=(std::nullptr_t) noexcept
	{
		delete m_Ptr;
		m_Ptr = nullptr;

		return *this;
	}

	template <typename T>
	template <typename U>
	constexpr UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr<U> &&other) noexcept
	{
		delete m_Ptr;
		m_Ptr = other.m_Ptr;
		other.m_Ptr = nullptr;

		return *this;
	}

	template <typename T>
	constexpr typename UniquePtr<T>::Pointer UniquePtr<T>::Get() const
	{
		return m_Ptr;
	}

	template <typename T>
	constexpr typename UniquePtr<T>::Pointer UniquePtr<T>::operator->() const
	{
		return m_Ptr;
	}

	template <typename T>
	constexpr typename UniquePtr<T>::Reference UniquePtr<T>::operator*() const
	{
		return *m_Ptr;
	}

	template <typename T>
	constexpr UniquePtr<T>::operator bool() const
	{
		return m_Ptr != nullptr;
	}

	template <typename T>
	constexpr void UniquePtr<T>::Reset(Pointer ptr)
	{
		delete m_Ptr;
		m_Ptr = ptr;
	}

	template <typename T>
	constexpr typename UniquePtr<T>::Pointer UniquePtr<T>::Release()
	{
		Pointer ptr = m_Ptr;
		m_Ptr = nullptr;

		return ptr;
	}

	template <typename T>
	constexpr void UniquePtr<T>::Swap(UniquePtr &other) noexcept
	{
		Pointer tmp = m_Ptr;
		m_Ptr = other.m_Ptr;
		other.m_Ptr = tmp;
	}

	template <typename T>
	template <typename... Args>
	constexpr UniquePtr<T> UniquePtr<T>::Create(Args &&...args)
	{
		return UniquePtr(new T(std::forward<Args>(args)...));
	}
} // namespace mlsl
